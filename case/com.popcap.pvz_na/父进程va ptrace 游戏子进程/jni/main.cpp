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

using namespace std;

#include "common.h"

pid_t _curPid = 0;

bool g_findAddrBase = false;
bool g_on_shared_so_load_Sucess = false;
std::mutex _readMutex;

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

void updateAddr()
{
	//if (g_findAddrBase)
	//	return;
	int pid = find_pid_of("com.popcap.pvz_na");
	LOGE("game id: %d", pid);
	readmaps(pid);
}

/*void updateAddr_safe()
{
	if(!xh_core_sigsegv_enable)
    {
        updateAddr();
    }
    else
    {    
        xh_core_sigsegv_flag = 1;
        if(0 == sigsetjmp(xh_core_sigsegv_env, 1))
        {
            updateAddr();
        }
        else
        {
            //LOGE("catch SIGSEGV when read or write mem\n");
        }
        xh_core_sigsegv_flag = 0;
    }
}*/

static char ori_buf1[]	= {0x03, 0x10, 0x82, 0xE0};//ADD R1, R2, R3				-528347133
static char buf1[]		= {0x03, 0x13, 0x82, 0xE0};//ADD R1, R2, R3, LSL #6		-528346365
static char ori_buf2[]	= {0x64, 0x10, 0x43, 0xE2};//SUB R1, R3, #0x64
static char buf2[]		= {0x00, 0x10, 0x43, 0xE2};//SUB R1, R3, #0x0
static char ori_buf3[]	= {0x14, 0x30, 0x1B, 0xE5};//LDR R3, [R11,#var_14]
static char buf3[]		= {0x00, 0x30, 0xA0, 0xE3};//MOV R3, #0 
static char ori_buf4[]	= {0x03, 0x20, 0x82, 0xE0};//ADD R2, R2, R3
static char buf4[]		= {0x03, 0x23, 0x82, 0xE0};//ADD R2, R2, R3, LSL #6

int dealCheat(int index, int open)
{
	void *data;
	void *data2;
	unsigned long addrTmp = 0;
	unsigned long addrTmp2 = 0;
	int value = 1;
	switch (index)
	{
	case 1://sun * 64
		addrTmp = _baseAddr + 0x1659D8;
		if (open == 1)
		{
			data = buf1;
		}
		else
		{
			data = ori_buf1;
		}
		LOGE("process_cheat 1\n");
		break;
	case 2://sun no drop
		addrTmp = _baseAddr + 0x165838;
		addrTmp2 = _baseAddr + 0x165C34;
		if (open == 1)
		{
			data = buf2;
			data2 = buf3;
		}
		else
		{
			data = ori_buf2;
			data2 = ori_buf3;
		}
		LOGE("process_cheat 2\n");
		break;
	case 3://coin * 64
		addrTmp = _baseAddr + 0x24C0D8;
		if (open == 1)
		{
			data = buf4;
		}
		else
		{
			data = ori_buf4;
		}
		LOGE("process_cheat 3\n");
		break;		
	case 4://speed
		
		break;		
	}
	LOGE("%x - %x - %x", _baseAddr, addrTmp, addrTmp2);
	
	LOGE("#########	attach\n");
	int status;
	if (ptrace(PTRACE_ATTACH, _curPid, NULL, NULL) == -1L)
	{
		LOGE("failed to attach\n");
        return -1;;
    }
	if (waitpid(_curPid, &status, 0) == -1 || !WIFSTOPPED(status))
	{
		LOGE("there was an error waiting for the target to stop.\n");
        return -1;
    }

	if (ptrace(PTRACE_POKEDATA, _curPid, (void *)(addrTmp), *(long *)(data)) == -1L)	
	{
		LOGE("memory allocation failed 1.\n");
		return -1;
    }
	if(index == 2 && addrTmp2)
	{
		if (ptrace(PTRACE_POKEDATA, _curPid, (void *)(addrTmp2), *(long *)(data2)) == -1L)	
		{
			LOGE("memory allocation failed 2.\n");
			return -1;
		}
	}
	
	LOGE("#########	detach\n");
	ptrace(PTRACE_DETACH, _curPid, 1, 0);
	return 0;
}
 
void process_cheat(int arg0, int arg1)
{
	if (0 == _curPid)
		return;

	if (0 == _baseAddr)
	{
		lock_guard<mutex> lock(_cheatCacheMutex);
		CheatCache obj(arg0, arg1);
		_cheatCache.push(obj);
		return;
	}
	
	pid_t fpid;
	fpid = fork();
	if (fpid < 0){
		LOGE("fork error");
	}
	else if(0 == fpid){
		LOGE("child process %d", getpid());
		dealCheat(arg0, arg1);
		_exit(0);
		LOGE("child process exit");
	}
	else{
		LOGE("parent process %d", getpid());
	}
}

void process_cheat_safe(int arg0, int arg1)
{
	if(!xh_core_sigsegv_enable)
    {
        process_cheat(arg0, arg1);
    }
    else
    {    
        xh_core_sigsegv_flag = 1;
        if(0 == sigsetjmp(xh_core_sigsegv_env, 1))
        {
             process_cheat(arg0, arg1);
        }
        else
        {
            LOGE("catch SIGSEGV when read or write mem\n");
        }
        xh_core_sigsegv_flag = 0;
    }
}

void* threadProc(void* param)
{
	while(!g_findAddrBase)
	{
		_timeStampHook.update();
		
		updateAddr();
		
		if (_timeStampHook.getElapsedSecond() < 0.25)
			usleep(250000); 
	}
	if (0 != _baseAddr)
	{
		lock_guard<mutex> lock(_cheatCacheMutex);
		while (!_cheatCache.empty())
		{
			CheatCache tmp = _cheatCache.front();
			process_cheat_safe(tmp._index, tmp._open);
			_cheatCache.pop();
		}
	}
	LOGE("exit thread\n");
	return NULL;
}

extern "C" {
	JNIEXPORT jint JNICALL Java_io_virtualapp_home_NativePtrace_gameStarted(JNIEnv *env, jclass clz);
	JNIEXPORT jint JNICALL Java_io_virtualapp_home_NativePtrace_soLoaded(JNIEnv *env, jclass clz);						
	JNIEXPORT void JNICALL Java_io_virtualapp_home_NativePtrace_doCheat(JNIEnv *env, jclass clz, jint arg0, jint arg1);
};

JNIEXPORT jint JNICALL Java_io_virtualapp_home_NativePtrace_gameStarted(JNIEnv *env, jclass clz)
{
	lock_guard<mutex> lock(_readMutex);
	_curPid = find_pid_of("com.popcap.pvz_na");
	LOGE("gameStarted: %d", _curPid);
	return _curPid;
}

JNIEXPORT jint JNICALL Java_io_virtualapp_home_NativePtrace_soLoaded(JNIEnv *env, jclass clz)
{
	lock_guard<mutex> lock(_readMutex);
	LOGE("soLoaded");
	updateAddr();
	if (g_findAddrBase)
		return 0;
	else
		return -1;
}

JNIEXPORT void JNICALL Java_io_virtualapp_home_NativePtrace_doCheat(JNIEnv *env, jclass clz, jint arg0, jint arg1)
{
	lock_guard<mutex> lock(_readMutex);
	LOGE("begin test ptrace");
	LOGE("va process %d", getpid());
	_curPid = find_pid_of("com.popcap.pvz_na");
	LOGE("game pid: %d", _curPid);
	updateAddr();
	dealCheat(arg0, arg1);//这里没有调用process_cheat，即没有fork子进程
	LOGE("end test ptrace");
}

