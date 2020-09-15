#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <android/log.h>
#include <elf.h>
#include <fcntl.h>
 
#define LOG_TAG "DEBUG"
#define LOGD(fmt, args...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, fmt, ##args)  

void appendToLogFile(const char *format, ...)
{
	char content[1024];
	memset(content, 0, 1024);

	va_list args;
	va_start(args, format);
	vsprintf(content, format, args);
#if 1
	strcat(content, "\n");
#endif 	
	va_end(args);

	FILE* log_file = fopen("/data/local/tmp/inlineHook.log", "a+");
	if (log_file != NULL) {
		fwrite(content, strlen(content), 1, log_file);
		fflush(log_file);
		fclose(log_file);
	}
}

 
int hook_entry_ori(char * a){
    appendToLogFile("Hook success, pid = %d\n", getpid());
    appendToLogFile("Hello %s\n", a);
    return 0;
}

//####################################################################
long dllBaseAddr;
long targetFun;
int (*f_il2cpp_runtime_invoke)(int a);
#include <dlfcn.h>    
#include <dirent.h>    
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
#include <stdbool.h>
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

bool readmaps(pid_t target, const char *soName)
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
			str = strstr(filename, soName);
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

#include "inlineHook.h"

//enum ele7en_status registerInlineHook(uint32_t target_addr, uint32_t new_addr, uint32_t **proto_addr)


void (*old_GetHurt)(void* arg0, int damage, void* source_object) = NULL;
//replace 的函数
void replaceFun(void* arg0, int damage, void* source_object)
{
	appendToLogFile("replaceFun arg0 %x\n", arg0);
	appendToLogFile("replaceFun arg %d\n", damage);//打印被hook函数的参数
	//old_GetHurt(arg0, damage, source_object);
}

//要被hook 的函数
/* void GetHurt(int damage, void* source_object)
{
	
}
 */
int hook()
{
	if (registerInlineHook((uint32_t) targetFun, (uint32_t) replaceFun, (uint32_t **) &old_GetHurt) != ELE7EN_OK) {
		appendToLogFile("registerInlineHook error\n");
        return -1;
    }
	appendToLogFile("registerInlineHook sucess\n");
	if (inlineHook((uint32_t) targetFun) != ELE7EN_OK) {
		appendToLogFile("inlineHook error\n");
        return -1;
    }
	appendToLogFile("inlineHook sucess\n");
    return 0;
}

int unHook()
{
    //if (inlineUnHook((uint32_t) puts) != ELE7EN_OK) {
	if (inlineUnHook((uint32_t) targetFun) != ELE7EN_OK) {
        return -1;
    }

    return 0;
}

int hook_entry(char * a){
    appendToLogFile("inject success, pid = %d\n", getpid());
    appendToLogFile("Hello %s\n", a);
	
	/*void* dll = NULL;
	char *error;
	dll = dlopen("libil2cpp.so", RTLD_NOW);
	if ((error = (char*)dlerror()) != NULL)  
		appendToLogFile("dlsym error dlopen:	%s\n", error);
	else
		appendToLogFile("dlsym dlopen sucess\n");
	
	if(dll==NULL)
		appendToLogFile("dlsym error dll == NULL\n");
	else
		appendToLogFile("dlsym dll != NULL\n");
	
	f_il2cpp_runtime_invoke = (int(*)(int))dlsym(dll, "l2cpp_runtime_invoke");
	if (f_il2cpp_runtime_invoke)
		appendToLogFile("find f_il2cpp_runtime_invoke.\n");
	else
		appendToLogFile("can not find f_il2cpp_runtime_invoke.\n");
	if ((error = (char*)dlerror()) != NULL)  
		appendToLogFile("dlsym f_il2cpp_runtime_invoke:	%s\n", error);
	
	Dl_info info;
	dladdr((void*)f_il2cpp_runtime_invoke, &info);
	appendToLogFile("f_il2cpp_runtime_invoke          Dl_info: %x	%x\n", info.dli_fbase, info.dli_saddr);*/
	
	pid_t target_pid;    
    target_pid = find_pid_of("com.knight.union.aligames");  
	appendToLogFile("target_pid %d\n", target_pid);	
	readmaps(target_pid, "libil2cpp.so");
	appendToLogFile("base addr %x\n", dllBaseAddr);	
	targetFun = dllBaseAddr + 0x70A5F0;
	
	hook();
	
    return 0;
}