#include <stdio.h>    
#include <stdlib.h>    
#include <asm/user.h>    
#include <asm/ptrace.h>    
#include <sys/ptrace.h>    
#include <sys/wait.h>    
#include <sys/mman.h>    
#include <dlfcn.h>    
#include <dirent.h>    
#include <unistd.h>    
#include <string.h>    
#include <elf.h>    
#include <android/log.h>    
#include <stdbool.h>
#include <unwind.h>
#include <dlfcn.h>
#include <getopt.h>
    
#if defined(__i386__)    
#define pt_regs         user_regs_struct    
#endif    
    
#define ENABLE_DEBUG 1    
    
#if ENABLE_DEBUG    
#define  LOG_TAG "INJECT"    
//#define  LOGD(fmt, args...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG, fmt, ##args)    
//#define DEBUG_PRINT(format,args...) \    
  //  LOGD(format, ##args)    
#else    
//#define DEBUG_PRINT(format,args...)    
#endif    

#define LOGD(fmt, args...)  fprintf(stderr, format, ##args) 
#define DEBUG_PRINT(format,args...) fprintf(stderr, format, ##args)    
    
#define CPSR_T_MASK     ( 1u << 5 )    
    
const char *libc_path = "/apex/com.android.runtime/lib/bionic/libc.so";    
const char *linker_path = "/apex/com.android.runtime/lib/bionic/libdl.so";    
    
long dllBaseAddr;
    
int find_pid_of(const char *process_name)    
{    
    int id;    
    pid_t pid = -1;    
    DIR* dir;    
    FILE *fp;    
    char filename[32];    
    char cmdline[256];    
    
    struct dirent * entry;    
    
    if (process_name == NULL)    
        return -1;    
    
    dir = opendir("/proc");    
    if (dir == NULL)    
        return -1;    
    
    while((entry = readdir(dir)) != NULL) {    
        id = atoi(entry->d_name);    
        if (id != 0) {    
            sprintf(filename, "/proc/%d/cmdline", id);    
            fp = fopen(filename, "r");    
            if (fp) {    
                fgets(cmdline, sizeof(cmdline), fp);    
                fclose(fp);    
    
                if (strcmp(process_name, cmdline) == 0) {    
                    /* process found */    
                    pid = id;    
                    break;    
                }    
            }    
        }    
    }    
    
    closedir(dir);    
    return pid;    
}    

#include <stdlib.h>
#include <string.h>
#include <errno.h>

/*
 * Avoid frequent malloc()/free() calls
 * (determined by getline() test on Linux)
 */
#define BUF_MIN 120

ssize_t getline(char **lineptr, size_t *n, FILE *stream)
{
    char *lptr;
    size_t len = 0;

    /* check for invalid arguments */
    if (lineptr == NULL || n == NULL) {
        errno = EINVAL;
        return -1;
    }

    lptr = fgetln(stream, &len);
    if (lptr == NULL) {
        /* invalid stream */
        errno = EINVAL;
        return -1;
    }

    /*
     * getline() returns a null byte ('\0') terminated C string,
     * but fgetln() returns characters without '\0' termination
     */
    if (*lineptr == NULL) {
        *n = BUF_MIN;
        goto alloc_buf;
    }

    /* realloc the original pointer */
    if (*n < len + 1) {
        free(*lineptr);

        *n = len + 1;
alloc_buf:
        *lineptr = malloc(*n);
        if (*lineptr == NULL) {
            *n = 0;
            return -1;
        }
    }

    /* copy over the string */
    memcpy(*lineptr, lptr, len);
    (*lineptr)[len] = '\0';

    /*
     * getline() and fgetln() both return len including the
     * delimiter but without the null byte at the end
     */
    return len;
}

bool readmaps(pid_t target)
{
    FILE *maps;
    char name[128], *line = NULL;
    char exelink[128];
    size_t len = 0;
    unsigned int code_regions = 0, exe_regions = 0;
    unsigned long prev_end = 0, load_addr = 0, exe_load = 0;
    bool is_exe = false;

#define MAX_LINKBUF_SIZE 256
    char linkbuf[MAX_LINKBUF_SIZE], *exename = linkbuf;
    int linkbuf_size;
    char binname[MAX_LINKBUF_SIZE];

    /* check if target is valid */
    if (target == 0)
        return false;

    /* construct the maps filename */
    snprintf(name, sizeof(name), "/proc/%u/maps", target);

    /* attempt to open the maps file */
    if ((maps = fopen(name, "r")) == NULL) 
	{
        fprintf(stderr, "failed to open maps file %s.\n", name);
        return false;
    }

    fprintf(stderr, "maps file located at %s opened.\n", name);

    /* get executable name */
    snprintf(exelink, sizeof(exelink), "/proc/%u/exe", target);
    linkbuf_size = readlink(exelink, exename, MAX_LINKBUF_SIZE - 1);
    if (linkbuf_size > 0)
    {
        exename[linkbuf_size] = 0;
    } else
	{
        /* readlink may fail for special processes, just treat as empty in
           order not to miss those regions */
        exename[0] = 0;
    }

    /* read every line of the maps file */
    while (getline(&line, &len, maps) != -1) 
	{
        unsigned long start, end;
        char read, write, exec, cow;
        int offset, dev_major, dev_minor, inode;

        /* slight overallocation */
        char filename[len];

        /* initialise to zero */
        memset(filename, '\0', len);

        /* parse each line */
        if (sscanf(line, "%lx-%lx %c%c%c%c %x %x:%x %u %[^\n]", &start, &end, &read,
                &write, &exec, &cow, &offset, &dev_major, &dev_minor, &inode, filename) >= 6) 
		{
			char* str = NULL;
			str = strstr(filename, "libil2cpp.so");
			//fprintf(stderr, "find %x ~ %x; %s\n", start, end, filename);
			if (str)
			{
				 fprintf(stderr, "---find %x ~ %x; %s\n", start, end, filename);
				 dllBaseAddr = start;
				 //dllBaseAddr = (void *)(start + 0x70A5F0 +1);
				 return true;
			}
           
        }
    }

    /* release memory allocated */
    free(line);
    fclose(maps);

    return true;
}
static void parse_parameters(int argc, char **argv, char **initial_commands, pid_t* target)
{
    struct option longopts[] = {
        {"pid",     1, NULL, 'p'},      
        {"command", 1, NULL, 'c'},      
        {NULL, 0, NULL, 0},
    };
    char *end;
    int optindex;

    switch (getopt_long(argc, argv, "p:c:", longopts, &optindex)) 
	{
        case 'p':
           *target = (pid_t) strtoul(optarg, &end, 0);

            break;
        case 'c':
            *initial_commands = optarg;
            break;
    }
}    

char *readline(const char *prompt)
{
    char *line = NULL;
    size_t n = 0;
    ssize_t bytes_read;

    printf("%s", prompt);
    fflush(stdout);
    bytes_read = getline(&line, &n, stdin);
    if (bytes_read > 0)
        line[bytes_read - 1] = '\0';  /* remove the trailing newline */

    return line;
}

bool getcommand(char **line)
{
    char prompt[64];
    bool success = true;

	snprintf(prompt, sizeof(prompt), "> ");
	
    while (true)
	{
        success = ((*line = readline(prompt)) != NULL);
        if (!success) 
		{
            /* EOF */
            if ((*line = strdup("__eof")) == NULL) 
			{
                fprintf(stderr, "sorry, there was a memory allocation error.\n");
                return false;
            }
            return true; /* exit immediately to not commit `__eof` to history */
        }

        if (strlen(*line)) 
		{
            break;
        }

        free(*line);
    }

    return true;
}

/*
dllBaseAddr + 70AEB0 + 4	护甲 不掉		00 00 A0 E3 1E FF 2F E1
dllBaseAddr + 70AF90 + 4	生命值 不掉		00 00 A0 E3 1E FF 2F E1
dllBaseAddr + 68BDC4 		子弹（气）不掉	C8 00 A0 E3				//mov r0,#200
在dnSpy中搜索数字时，选择 数字/字符串 的方式搜索，搜索70AEB0时不要带0x
*/
int wudi(pid_t target_pid, int open)
{
	int status;
	fprintf(stderr, "#########	attach\n");
	if (ptrace(PTRACE_ATTACH, target_pid, NULL, NULL) == -1L)
	{
		fprintf(stderr, "failed to attach\n");
        return -1;
    }
	if (waitpid(target_pid, &status, 0) == -1 || !WIFSTOPPED(status))
	{
		fprintf(stderr, "there was an error waiting for the target to stop.\n");
        return -1;
    }

	readmaps(target_pid);
	
	char ori_buf[]= {0x18, 0xB0, 0x8D, 0xE2, 0x08, 0xD0, 0x4D, 0xE2};
	char buf[] = { 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1 };
	void *data;
	if (open == 1)
	{
		data = buf;
	}
	else
	{
		data = ori_buf;
	}

	//+4的原因是 需要跳掉 STMFD SP!, {R4-R9,R11,LR}指令
	if (ptrace(PTRACE_POKEDATA, target_pid, (void *)(dllBaseAddr + 0x70A5F0 + 4), *(long *)(data + 0)) == -1L)	
	{
		fprintf(stderr, "memory allocation failed 1.\n");
		return -1;
    }
	
	if (ptrace(PTRACE_POKEDATA, target_pid, (void *)(dllBaseAddr + 0x70A5F0 + 4+ 4), *(long *)(data + 4)) == -1L)
	{
		fprintf(stderr, "memory allocation failed 2.\n");
		return -1;
    }
	
	fprintf(stderr, "#########	detach\n");
	ptrace(PTRACE_DETACH, target_pid, 1, 0);
}


int (*f_il2cpp_runtime_invoke)(int a);

/*
./crackWudi -c com.knight.union.aligames	指定进程的包名，通过包名去获取进程id
./crackWudi -p 9763	直接指定进程id
*/
int main(int argc, char** argv) {    
    pid_t target_pid = 0;    
	char *packageName = NULL;
	parse_parameters(argc, argv, &packageName, &target_pid);
	
	fprintf(stderr, "argv packageName %s\n", packageName);
	fprintf(stderr, "argv target_pid %d\n", target_pid);
	
	if (packageName)
		target_pid = find_pid_of(packageName); 
	else if (target_pid == 0)
		target_pid = find_pid_of("com.knight.union.aligames");   
	fprintf(stderr, "deal pid %d\n", target_pid);
	
	
	while (true) 
	{
		char *line = NULL;
		if (getcommand(&line) == false) 
		{
            fprintf(stderr, "failed to read in a command.\n");
			break;
        }
		if (line)
		{
			if (strcmp(line, "exit") == 0)
			{
				break;
			}
			if (strcmp(line, "1") == 0)
			{
				wudi(target_pid, 1);
			}
			else
			{
				wudi(target_pid, 0);
			}
		}
		
	}
	
	
	
	/*void* dll = NULL;
	char *error;
	dll = dlopen("/data/app/com.knight.union.aligames-v6TXD-HqKFV2Oy6fJM5KvA==/lib/arm/libil2cpp.so", RTLD_NOW);
	if ((error = (char*)dlerror()) != NULL)  
		fprintf(stderr, "dlsym error dlopen:	%s\n", error);
	else
		fprintf(stderr, "dlsym dlopen sucess\n");
	
	if(dll==NULL)
		fprintf(stderr, "dlsym error dll == NULL\n");
	else
		fprintf(stderr, "dlsym dll != NULL\n");
	
	f_il2cpp_runtime_invoke = (int(*)(int))dlsym(dll, "l2cpp_runtime_invoke");
	if (f_il2cpp_runtime_invoke)
		fprintf(stderr, "find f_il2cpp_runtime_invoke.\n");
	else
		fprintf(stderr, "can not find f_il2cpp_runtime_invoke.\n");
	if ((error = (char*)dlerror()) != NULL)  
		fprintf(stderr, "dlsym f_il2cpp_runtime_invoke:	%s\n", error);
	
	Dl_info info;
	dladdr((void*)f_il2cpp_runtime_invoke, &info);
	fprintf(stderr, "f_il2cpp_runtime_invoke          Dl_info: %x	%x\n", info.dli_fbase, info.dli_saddr);*/
	
	/*fprintf(stderr, "#########	write mem\n");
	long data;
	void *addr;
	char *endptr;
	const char *strAddress;
	addr = (void *)strtoll(strAddress, &endptr, 16);
	
	if (ptrace(PTRACE_POKEDATA, target_pid, addr, data) == -1L)
	{
		fprintf(stderr, "memory allocation failed.\n");
		return -1;
    }*/



    return 0;  
}    