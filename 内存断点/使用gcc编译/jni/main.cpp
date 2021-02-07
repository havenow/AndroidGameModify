#include <string.h>
#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <vector>
#include <queue>
#include <mutex>
#include <thread>

#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ptrace.h>    
#include <sys/wait.h>    
#include <mutex> 
#include <assert.h>

using namespace std;

#include "common.h"

pid_t _curPid = 0;

bool g_findAddrBase = false;
bool g_on_shared_so_load_Sucess = false;
std::mutex _readMutex;
int g_tagetAddr = 0;

class CheatCache
{
public:
	CheatCache(int index, int open)
	{
		_index = index;
		_open = open;
	}
	int _index = 0;
	int _open = 0;
};

unsigned long _baseAddr = 0;
queue<CheatCache> _cheatCache;
mutex _cheatCacheMutex;

#include <stdarg.h>
void dumpMem(long data)
{
	FILE* log_file = fopen("/data/local/tmp/memRegion", "a+");
	if (log_file != NULL) 
	{
		fwrite(&data, 1, 4, log_file);
		fflush(log_file);
		fclose(log_file);
	}
}

//------------------signal handler for SIGSEGV
#define XH_ERRNO_UNKNOWN 1001
#include <setjmp.h>
#include <errno.h>
static int              xh_core_sigsegv_enable = 1; //enable by default
static struct sigaction xh_core_sigsegv_act_old;
static volatile int     xh_core_sigsegv_flag = 0;//通过判断 flag 的值，来判断当前线程逻辑是否在危险区域中
static sigjmp_buf       xh_core_sigsegv_env;
static void xh_core_sigsegv_handler(int sig)
{
    (void)sig;
    
    if(xh_core_sigsegv_flag)
        siglongjmp(xh_core_sigsegv_env, 1);
    else
        sigaction(SIGSEGV, &xh_core_sigsegv_act_old, NULL);
}
static int xh_core_add_sigsegv_handler()
{
    struct sigaction act;

    if(!xh_core_sigsegv_enable) return 0;
    
    if(0 != sigemptyset(&act.sa_mask)) return (0 == errno ? XH_ERRNO_UNKNOWN : errno);
    act.sa_handler = xh_core_sigsegv_handler;
    
    if(0 != sigaction(SIGSEGV, &act, &xh_core_sigsegv_act_old))
        return (0 == errno ? XH_ERRNO_UNKNOWN : errno);

    return 0;
}
static void xh_core_del_sigsegv_handler()
{
    if(!xh_core_sigsegv_enable) return;
    
    sigaction(SIGSEGV, &xh_core_sigsegv_act_old, NULL);
}

/*
bool init_ok = false;

//register signal handler
if(0 == xh_core_add_sigsegv_handler()) 
	init_ok = true;

//unregister the sig handler
if (init_ok == true)
	xh_core_del_sigsegv_handler();
*/

static void test()
{
    if(!xh_core_sigsegv_enable)
    {
        //do something;
    }
    else
    {    
        xh_core_sigsegv_flag = 1;
        if(0 == sigsetjmp(xh_core_sigsegv_env, 1))
        {
            //do something;
        }
        else
        {
            LOGE("catch SIGSEGV when read or write mem\n");
        }
        xh_core_sigsegv_flag = 0;
    }
}
//--------------------------------------end

#include "timestamp.hpp"
CTimestamp _timeStampHook;
static std::thread _thread;

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
        *lineptr = (char*)malloc(*n);
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
	//LOGE("begin readmaps.\n");
	size_t anonymnousMemSize = 0;
	
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
    /*if (target == 0)
        return false;*/

    /* construct the maps filename */
    snprintf(name, sizeof(name), "/proc/%u/maps", target);
	//snprintf(name, sizeof(name), "/proc/self/maps");

    /* attempt to open the maps file */
    if ((maps = fopen(name, "r")) == NULL) 
	{
        LOGE("failed to open maps file %s.\n", name);
        return false;
    }

    //LOGE("maps file located at %s opened.\n", name);

    /* get executable name */
    snprintf(exelink, sizeof(exelink), "/proc/%u/exe", target);
	//snprintf(exelink, sizeof(exelink), "/proc/self/exe");
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
			str = strstr(filename, "libpvz.so");
			//fprintf(stderr, "find %x ~ %x; %s\n", start, end, filename);
			if (str)
			{
				 fprintf(stderr, "---find %x ~ %x; %s\n", start, end, filename);
				 _baseAddr = start;
				 g_findAddrBase = true;
				 return true;
			}  
        }	
    }

    /* release memory allocated */
    free(line);
    fclose(maps);

    return true;
}

#include <sys/mman.h>
#define PAGE_START(addr) ((addr) & PAGE_MASK)
#define PAGE_END(addr)   (PAGE_START(addr) + PAGE_SIZE)

char g_szDefaultSocketName[64] = "cheat";
int g_socket = 0;
int g_port = 1166;
bool g_cheatSDKFinish = false;

void memWatch(int addr, int nWrite)
{
	int pagesizeConf;
	pagesizeConf = sysconf(_SC_PAGE_SIZE);
	int pageSize = PAGE_SIZE;
	int pageMask = PAGE_MASK;
	LOGE("pagesizeConf:%d, pageSize:%d, pageMask:%d\n", pagesizeConf, pageSize, pageMask);//4096 4096 -4096
	
	//uintptr_t  addr = 0x9E06D2A4;
	void* startAddr = (void *)PAGE_START(addr);
	void* endAddr = (void *)(PAGE_START(addr) + PAGE_SIZE);
	LOGE("addr: %x ~ %x ~ %x\n", startAddr, addr, endAddr);//addr: 9e06d000 ~ 9e06d2a4 ~ 9e06e000
	int ret;
	if (nWrite == 1)
		ret = mprotect((void *)PAGE_START(addr), PAGE_SIZE, PROT_READ | PROT_WRITE);
	else
		ret = mprotect((void *)PAGE_START(addr), PAGE_SIZE, PROT_READ);
	if (ret == -1)
		LOGE("mprotect fail %d\n", errno);
	else
		LOGE("mprotect sucess\n");
}

int start_server(char* sockname)
{
	int fd = socket(AF_INET, SOCK_STREAM, 0);

	if (fd < 0)
	{
		LOGE("creating socket");
		return fd;
	}

	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(g_port);

	int opt;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (void*)&opt, sizeof(opt));

	if (bind(fd, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0)
	{
		LOGE("binding socket");
		close(fd);
		return -1;
	}

	listen(fd, 1);

	return fd;
}

void process_cmd(char* io_buffer)
{
	LOGE("process_cmd io_buffer:	%s\n", io_buffer);
	int index;
	char* nextPtr;
	void* addr;
	int data;
	char* cursor = io_buffer;
	cursor += 1;
	int status;
	switch (io_buffer[0])
	{
	case 'D':
		{
			char strCheatCmd[128];
			memset(strCheatCmd, '0', 128);
			sscanf(io_buffer, "D: %s", strCheatCmd);
		}
		break;
	case 'E':
		{
			int targetAddr = 0;
			int nWrite = 0;
			sscanf(io_buffer, "E: %d %d", &targetAddr, &nWrite);
			g_tagetAddr = targetAddr;
			memWatch(targetAddr, nWrite);
		}
		break;
	case 'Z':
		{
			g_cheatSDKFinish = true;

		}
		break;
	default:
		break;
	}
}


static pthread_t cmd_thread;
void* threadCmdProc(void* param)
{
	char* sockname = g_szDefaultSocketName;
	struct sockaddr_un client_addr;
	socklen_t client_addr_length = sizeof(client_addr);

	int server_fd = start_server(sockname);
	if (server_fd < 0)
	{
		LOGE("start server failed:Unable to start server on %s\n", sockname);
		LOGE("maybe have already start a server.\n");
		return NULL;
	}
	LOGE( "start server succeeded\n");
	while (1)
	{
		if (g_cheatSDKFinish)
			break;
		int client_fd = accept(server_fd, (struct sockaddr*) NULL, NULL);

		if (client_fd < 0)
		{
			LOGE("accepting client exit(1)\n");
			//exit(1);
			continue;
		}

		LOGE("Connection established\n");

		char io_buffer[80] = { 0 };
		int io_length = 0;
		char* cursor;
		long int contact, x, y, pressure;

		while (1) {
			io_length = 0;

			while (io_length < sizeof(io_buffer)
					&& recv(client_fd, &io_buffer[io_length], 1, 0) == 1) {
				if (io_buffer[io_length++] == '\n') {
					break;
				}
			}

			if (io_length <= 0) {
				LOGE("judgement: client %d have exited.\n", client_fd);
				break;
			}

			if (io_buffer[io_length - 1] != '\n') {
				continue;
			}

			if (io_length == 1) {
				continue;
			}

			process_cmd(io_buffer);
		}

		LOGE("client_fd %d Connection closed\n", client_fd);
		close(client_fd);
	}

	close(server_fd);
	LOGE("exit cheat thread\n");
    return NULL;
}


void sigsegv_handler_mem(int signo)
{
	if (signo == SIGSEGV) {
		LOGE("sigsegv_handler_mem SIGSEGV");
		callstackLogcat(ANDROID_LOG_ERROR, "inline");
	} 
}

//https://www.jianshu.com/p/f445bfeea40a		Linux 信号（signal）
//https://stackoverflow.com/questions/8115192/android-ndk-getting-the-backtrace 	Android NDK: getting the backtrace
//https://blog.csdn.net/taohongtaohuyiwei/article/details/105147933

//--------------
static void sig_usr(int sig)
{
	if (sig == SIGSEGV) {
		LOGE("sig_sur SIGSEGV");
		
	} 
}

 // siginfo_t {
               // int      si_signo;    /* Signal number */
               // int      si_errno;    /* An errno value */
               // int      si_code;     /* Signal code */
               // int      si_trapno;   /* Trap number that caused
                                        // hardware-generated signal
                                        // (unused on most architectures) */
               // pid_t    si_pid;      /* Sending process ID */
               // uid_t    si_uid;      /* Real user ID of sending process */
               // int      si_status;   /* Exit value or signal */
               // clock_t  si_utime;    /* User time consumed */
               // clock_t  si_stime;    /* System time consumed */
               // sigval_t si_value;    /* Signal value */
               // int      si_int;      /* POSIX.1b signal */
               // void    *si_ptr;      /* POSIX.1b signal */
               // int      si_overrun;  /* Timer overrun count; POSIX.1b timers */
               // int      si_timerid;  /* Timer ID; POSIX.1b timers */
               // void    *si_addr;     /* Memory location which caused fault */
               // int      si_band;     /* Band event */
               // int      si_fd;       /* File descriptor */
// }
#include "libunwind.h"
struct BacktraceState {
    const ucontext_t*   signal_ucontext;
    size_t              address_count = 0;
    static const size_t address_count_max = 30;
    uintptr_t           addresses[address_count_max] = {};

    BacktraceState(const ucontext_t* ucontext) : signal_ucontext(ucontext) {}

    bool AddAddress(uintptr_t ip) {
        // No more space in the storage. Fail.
        if (address_count >= address_count_max)
            return false;

        // Add the address to the storage.
        addresses[address_count++] = ip;
        return true;
    }
};

/* void CaptureBacktraceUsingLibUnwind(BacktraceState* state) {
    assert(state);

    // Initialize unw_context and unw_cursor.
    unw_context_t unw_context = {};
    unw_getcontext(&unw_context);
    unw_cursor_t  unw_cursor = {};
    unw_init_local(&unw_cursor, &unw_context);

    // Get more contexts.
    const ucontext_t* signal_ucontext = state->signal_ucontext;
    assert(signal_ucontext);
    const sigcontext* signal_mcontext = &(signal_ucontext->uc_mcontext);
    assert(signal_mcontext);

    // Set registers.
    unw_set_reg(&unw_cursor, UNW_ARM_R0,  signal_mcontext->arm_r0);
    unw_set_reg(&unw_cursor, UNW_ARM_R1,  signal_mcontext->arm_r1);
    unw_set_reg(&unw_cursor, UNW_ARM_R2,  signal_mcontext->arm_r2);
    unw_set_reg(&unw_cursor, UNW_ARM_R3,  signal_mcontext->arm_r3);
    unw_set_reg(&unw_cursor, UNW_ARM_R4,  signal_mcontext->arm_r4);
    unw_set_reg(&unw_cursor, UNW_ARM_R5,  signal_mcontext->arm_r5);
    unw_set_reg(&unw_cursor, UNW_ARM_R6,  signal_mcontext->arm_r6);
    unw_set_reg(&unw_cursor, UNW_ARM_R7,  signal_mcontext->arm_r7);
    unw_set_reg(&unw_cursor, UNW_ARM_R8,  signal_mcontext->arm_r8);
    unw_set_reg(&unw_cursor, UNW_ARM_R9,  signal_mcontext->arm_r9);
    unw_set_reg(&unw_cursor, UNW_ARM_R10, signal_mcontext->arm_r10);
    unw_set_reg(&unw_cursor, UNW_ARM_R11, signal_mcontext->arm_fp);
    unw_set_reg(&unw_cursor, UNW_ARM_R12, signal_mcontext->arm_ip);
    unw_set_reg(&unw_cursor, UNW_ARM_R13, signal_mcontext->arm_sp);
    unw_set_reg(&unw_cursor, UNW_ARM_R14, signal_mcontext->arm_lr);
    unw_set_reg(&unw_cursor, UNW_ARM_R15, signal_mcontext->arm_pc);

    unw_set_reg(&unw_cursor, UNW_REG_IP, signal_mcontext->arm_pc);
    unw_set_reg(&unw_cursor, UNW_REG_SP, signal_mcontext->arm_sp);

    // unw_step() does not return the first IP,
    // the address of the instruction which caused the crash.
    // Thus let's add this address manually.
    state->AddAddress(signal_mcontext->arm_pc);

    // Unwind frames one by one, going up the frame stack.
    while (unw_step(&unw_cursor) > 0) {
        unw_word_t ip = 0;
        unw_get_reg(&unw_cursor, UNW_REG_IP, &ip);

        bool ok = state->AddAddress(ip);
        if (!ok)
            break;
    }
} */

static void sigaction_usr(int sig, siginfo_t* info, void* ucontext)
{
	const ucontext_t* signal_ucontext = (const ucontext_t*)ucontext;
    assert(signal_ucontext);
	
	//common.cpp获取backtrace 是使用unwind.h Symbolization is possible via dladdr().
	
	// Android NDK r16b contains "libunwind.a" for armeabi-v7a ABI.
	//BacktraceState backtrace_state(signal_ucontext);
    //CaptureBacktraceUsingLibUnwind(&backtrace_state);
	
	if (sig == SIGSEGV) {
		//LOGE("sigaction_usr SIGSEGV");
		if (g_tagetAddr == reinterpret_cast<int>(info->si_addr))
			LOGE("%x", info->si_addr);//只打印导致SIGSEGV错误的内存地址
		//如果打印backtrace，可以调用common.cpp中的函数
	} 
}

typedef union {
	JNIEnv* env;
	void* venv;
} UnionJNIEnvToVoid;

__attribute__((visibility("default")))
jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	UnionJNIEnvToVoid uenv;
	uenv.venv = NULL;
	jint result = -1;
	JNIEnv* env = NULL;

	appendToLogFile("JNI_OnLoad.\n");
	LOGE("JNI_OnLoad.\n");

	if (vm->GetEnv(&uenv.venv, JNI_VERSION_1_4) != JNI_OK) {
		appendToLogFile("ERROR: GetEnv failed.\n");
		return result;
	}
	env = uenv.env;
	
	if (getuid() != 0)
        LOGE("*** RUNNING AS unROOT. ***\n");
	else
		LOGE("*** RUNNING AS ROOT. ***\n");
	
	/*if(0 == xh_core_add_sigsegv_handler()) 
		LOGE("register sigsegv_handler ok");
	else
		LOGE("register sigsegv_handler fail");*/
	
	//signal(SIGSEGV, sigsegv_handler_mem);
	struct sigaction act;
	act.sa_flags = SA_SIGINFO;
	act.sa_sigaction = sigaction_usr;   //信号处理函数
	
	sigaction(SIGSEGV, &act, NULL);
	
	
	LOGE("create cmd thread.\n");
	pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_create(&cmd_thread, &attr, threadCmdProc, NULL);

	result = JNI_VERSION_1_4;
	return result;
}



int main(int argc, char** argv)
{
	if (getuid() != 0)
        fprintf(stderr, "*** RUNNING AS unROOT. ***\n");
	else
		fprintf(stderr, "*** RUNNING AS ROOT. ***\n");
	
	bool bForTest[2] = {false, false};
	struct sockaddr_in addr;
	g_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (g_socket < 0)
	{
		fprintf(stderr, "creating socket error\n");
		return 1;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET; //设置为IP通信
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(g_port);

	if (connect(g_socket, (struct sockaddr *) &addr, sizeof(addr)) < 0)
	{
		fprintf(stderr, "connect socket error %d\n", errno);
		close(g_socket);
		return 1;
	}
	fprintf(stderr, "connect socket sucess %d\n", g_socket);
		
	while (1)
	{
		fprintf(stderr, "input commonds>");
		char cmdBuf[128] = {};
		scanf("%s", cmdBuf);
		if (0 == strcmp(cmdBuf, "exit"))
		{
			fprintf(stderr, "exit cmd\n");
			break;
		}
		else if (0 == strcmp(cmdBuf, "help"))
		{
			;
		}
		else if (0 == strcmp(cmdBuf, "sdkFinish"))
		{
			char cmd[80] = { 0 };
			int io_length;
			io_length = snprintf(cmd, sizeof(cmd), "Z\n");
			send(g_socket, cmd, io_length, 0);
			
			fprintf(stderr, "exit cmd\n");
			break;
		}
		else
		{
			int addr = 0;
			int write = 0;
			sscanf(cmdBuf, "%d,%d", &addr, &write);
			fprintf(stderr, "data:%s,%d,%d\n", cmdBuf, addr, write);
			char cmd[80] = { 0 };
			int io_length;
			io_length = snprintf(cmd, sizeof(cmd), "E: %d %d\n", addr, write);
			fprintf(stderr, "send cmd:%s\n", cmd);
			send(g_socket, cmd, io_length, 0);
		}
	}
	
	
	fprintf(stderr, "socket close %d\n", g_socket);
	close(g_socket);
	return 0;
}

