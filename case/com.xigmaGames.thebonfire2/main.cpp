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

using namespace std;

#include "common.h"

pid_t _curPid = 0;

bool g_findAddrBase = false;
bool g_on_shared_so_load_Sucess = false;
float _speed = 0.0f;
bool g_bHealthSwitch = false;

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
    //snprintf(name, sizeof(name), "/proc/%u/maps", target);
	snprintf(name, sizeof(name), "/proc/self/maps");

    /* attempt to open the maps file */
    if ((maps = fopen(name, "r")) == NULL) 
	{
        LOGE("failed to open maps file %s.\n", name);
        return false;
    }

    //LOGE("maps file located at %s opened.\n", name);

    /* get executable name */
    //snprintf(exelink, sizeof(exelink), "/proc/%u/exe", target);
	snprintf(exelink, sizeof(exelink), "/proc/self/exe");
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
				 LOGE("---find %x ~ %x; %s, %c, %c, %c\n", start, end, filename, read, write, exec);
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
	if (g_findAddrBase)
		return;
	
	readmaps(0);
}

static char ori_buf1[]	= {0x0a, 0x20, 0x82, 0xe0};//add    r2,  r2,  r10				-528343030
static char buf1_16[]	= {0x0a, 0x22, 0x82, 0xe0};//add    r2,  r2,  r10, LSL #4		-528342518	

static char ori_buf21[]	= {0x00, 0x00, 0x50, 0xe3};//CMP             R0, #0
static char buf21[]		= {0x28, 0x00, 0x96, 0xe5};//LDR             R0, [R6,#0x28]		-443154392
static char ori_buf22[]	= {0x07, 0x00, 0xa0, 0xd1};//MOVLE           R0, R7
static char buf22[]		= {0x28, 0x00, 0x96, 0xe5};//LDR             R0, [R6,#0x28]		-443154392

/*
il2cpp:005DBE9C 34 00 96 E5                 LDR             R0, [R6,#0x34]	-443154380
il2cpp:005DBEA0 01 00 50 E3                 CMP             R0, #1			-481296383
il2cpp:005DBEA4 01 00 40 A2                 SUBGE           R0, R0, #1		-1572863999
il2cpp:005DBEA8 34 00 86 A5                 STRGE           R0, [R6,#0x34]	-1517944780
*/
static char ori_buf3[]	= {0x34, 0x00, 0x96, 0xe5, 0x01, 0x00, 0x50, 0xE3, 0x01, 0x00, 0x40, 0xA2, 0x34, 0x00, 0x86, 0xA5}; 

/*
LDR           R0, [R6,#0x30]	30 00 96 e5	-443154384
MOV	r0,r0			00 00 a0 e1	-509607936
MOV	r0,r0			00 00 a0 e1	-509607936
STR           R0, [R6,#0x34]	34 00 86 e5	-444202956
*/
static char buf3[]		= {0x30, 0x00, 0x96, 0xe5, 0x00, 0x00, 0xa0, 0xE1, 0x00, 0x00, 0xa0, 0xE1, 0x34, 0x00, 0x86, 0xE5}; 


#define PAGE_START(addr) ((addr) & PAGE_MASK)
int dealCheat(int index, int open)
{
	void *data;
	void *data2;
	void *data3;
	void *data4;
	unsigned long addrTmp = 0;
	unsigned long addrTmp2 = 0;
	int value = 1;
	switch (index)
	{
	case 1://Resource * 16
		addrTmp = _baseAddr + 0x461508;
		if (open == 1)
		{
			data = buf1_16;
		}
		else
		{
			data = ori_buf1;
		}
		LOGE("process_cheat 1 Resource * 16\n");
		break;
	case 2://Hunger: Stomach Fill
		addrTmp = _baseAddr + 0x5DC128;
		addrTmp2 = _baseAddr + 0x5DC12C;
		if (open == 1)
		{
			data = buf21;
			data2 = buf22;
		}
		else
		{
			data = ori_buf21;
			data2 = ori_buf22;
		}
		LOGE("process_cheat 2 Hunger: Stomach Fill\n");
		break;
	case 3://Energy: max Energy
		addrTmp = _baseAddr + 0x5DBE9C;
		if (open == 1)
		{
			data = buf3;
		}
		else
		{
			data = ori_buf3;
		}
		LOGE("process_cheat 3 Energy: max Energy\n");
		break;		
	case 4://speed
		switch (open)
		{
			case 1:
				_speed = 1.0f;
				break;
			case 2:
				_speed = 2.0f;
				break;
			case 3:
				_speed = 4.0f;
				break;
			case 4:
				_speed = 8.0f;
				break;
			case 5:
				_speed = 0.5f;
				break;
		}
		LOGE("process_cheat 4 speed %lf\n", _speed);
		break;
	case 5://health: max health 
		if (open == 1)
			g_bHealthSwitch = true;
		else
			g_bHealthSwitch = false;
		LOGE("process_cheat 5 health %d\n", g_bHealthSwitch);
		break;
	}
	
	if (index == 4 || index == 5)
		return 0;
	
	LOGE("%x - %x - %x", _baseAddr, addrTmp, addrTmp2);
	
	int pageSize = PAGE_SIZE;
	int pageMask = PAGE_MASK;
	LOGE("PAGE_SIZE:%d, PAGE_MASK:%d\n", pageSize, pageMask);//4096, 4096
	
	void* startAddr = (void *)PAGE_START(addrTmp);
	void* endAddr = (void *)(PAGE_START(addrTmp) + PAGE_SIZE);
	LOGE("addr: %x ~ %x ~ %x\n", startAddr, addrTmp, endAddr);//addr: bf212000 ~ bf2120d8 ~ bf213000
	int ret;
	ret = mprotect((void *)PAGE_START(addrTmp), PAGE_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC);
	if (ret == -1)
		LOGE("mprotect fail %d\n", errno);
	else
		LOGE("mprotect sucess\n");
	
	switch(index)
	{
		case 1:
			memcpy(addrTmp, data, 4);
			break;
		case 2:
			memcpy(addrTmp, data, 4);
			memcpy(addrTmp2, data2, 4);
			break;
		case 3:
			memcpy(addrTmp, data, 16);
			break;
	}
	
	ret = mprotect((void *)PAGE_START(addrTmp), PAGE_SIZE, PROT_READ | PROT_EXEC);
	if (ret == -1)
		LOGE("mprotect fail %d\n", errno);
	else
		LOGE("mprotect sucess\n");
	
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
	
	dealCheat(arg0, arg1);
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

const int IL_SCRI = 5;
const char * il2cpp_script[IL_SCRI] = {
		"il2cpp_runtime_invoke",
		"il2cpp_method_get_class",
		"il2cpp_class_get_image",
		"il2cpp_class_from_name",
		"il2cpp_class_get_method_from_name" };
		
typedef char *(*IL2CPP_METHOD_FROM_NAME)(void * class_object, char *name, int agrs);
typedef void *(*IL2CPP_METHOD_GET_CLASS)(void *method);
typedef void *(*IL2CPP_CLASS_GET_IMAGE)(void *class_object);
typedef void *(*IL2CPP_CLASS_FROM_NAME)(void *image, char *space, char *name);

IL2CPP_METHOD_FROM_NAME il2cpp_method_from_name;
IL2CPP_METHOD_GET_CLASS il2cpp_method_get_class;
IL2CPP_CLASS_GET_IMAGE il2cpp_class_get_image;
IL2CPP_CLASS_FROM_NAME il2cpp_class_from_name;

void * il2cpp_time_scale_method;
void *(*IL2CPP_RUN_TIME_INVOKE)(void *method, void *obj, void **params, void **exc);

void* il2cpp_run_time_invoke(void *method, void *obj, void **params, void **exc) {
	void *any_class = il2cpp_method_get_class(method);
	if (any_class != NULL) {
		void * image_name = il2cpp_class_get_image(any_class);
		if (image_name != NULL) {
			void *target_class = il2cpp_class_from_name(image_name, "UnityEngine", "Time");
			if (target_class != NULL) {
				il2cpp_time_scale_method = il2cpp_method_from_name(target_class, "set_timeScale", 1);
				if (il2cpp_time_scale_method != NULL) {
					void *args[1];
					float scale = _speed;
					args[0] = &scale;
					float* tmp = reinterpret_cast<float*>(args[0]);
					if (_speed > 0.1f)
						IL2CPP_RUN_TIME_INVOKE(il2cpp_time_scale_method, NULL, args, NULL);	
					//LOGE("set_timeScale %lf", _speed);
				} 
			} 
		} 
	} 
	return IL2CPP_RUN_TIME_INVOKE(method, obj, params, exc);
}

int(*REDUCE_HEALTH)(int* a1, int a2, int a3);
int reduceHealth(int* a1, int a2, int a3) 
{
	//return REDUCE_HEALTH(a1, a2, a3);
	int intelligence =  a1[7];
	int expereience =  a1[8];
	int ret = REDUCE_HEALTH(a1, a2, a3);
	if (g_bHealthSwitch == true && (intelligence == -1 && expereience == 0) == false)
	{
		//LOGE("set health to max %d", a1[14]);
		a1[15] = a1[14];
	}
	return ret;
}

#include "inlinehook\inlineHook.h"
void* threadProc(void* param)
{
	while(!g_findAddrBase)
	{
		_timeStampHook.update();
		
		updateAddr();
		
		if (_timeStampHook.getElapsedSecond() < 0.25)
			usleep(250000); 
	}
	
	il2cpp_method_get_class = reinterpret_cast<IL2CPP_METHOD_GET_CLASS>(_baseAddr + 0x286A90);
	il2cpp_class_get_image = reinterpret_cast<IL2CPP_CLASS_GET_IMAGE>(_baseAddr + 0x2865A0);
	il2cpp_class_from_name = reinterpret_cast<IL2CPP_CLASS_FROM_NAME>(_baseAddr + 0x286524);
	il2cpp_method_from_name = reinterpret_cast<IL2CPP_METHOD_FROM_NAME>(_baseAddr + 0x28654C);
	
	long fun_il2cpp_scr_run_time_invoke = _baseAddr + 0x286B68;
	LOGE("fun_il2cpp_scr_run_time_invoke %x", fun_il2cpp_scr_run_time_invoke);
	if (registerInlineHook((uint32_t) fun_il2cpp_scr_run_time_invoke, (uint32_t) (il2cpp_run_time_invoke), (uint32_t **) &IL2CPP_RUN_TIME_INVOKE) != ELE7EN_OK) 
	{
		LOGE("registerInlineHook fun_il2cpp_scr_run_time_invoke failure\n");
	}
	if (inlineHook((uint32_t) fun_il2cpp_scr_run_time_invoke) != ELE7EN_OK) {
		LOGE("inlineHook fun_il2cpp_scr_run_time_invoke failure\n");
	} 
	LOGE("hook il2cpp_scr_run_time_invoke finish\n");
	
	//----------
	long fun_ReduceHealth  = _baseAddr +  0x5E5228;
	LOGE("fun_ReduceHealth %x", fun_ReduceHealth);
	if (registerInlineHook((uint32_t) fun_ReduceHealth, (uint32_t) (reduceHealth), (uint32_t **) &REDUCE_HEALTH) != ELE7EN_OK) 
	{
		LOGE("registerInlineHook fun_ReduceHealth failure\n");
	}
	if (inlineHook((uint32_t) fun_ReduceHealth) != ELE7EN_OK) {
		LOGE("inlineHook fun_ReduceHealth failure\n");
	} 
	LOGE("hook ReduceHealth finish\n");
	
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

#define EXPORT_FUNC __attribute__((visibility("default")))

extern "C"
{
	EXPORT_FUNC void setLibPath(std::string path)
	{
		
	}
	
	EXPORT_FUNC void on_shared_so_load(std::string soName, void* handle)
	{
		if (soName.find("com.xigmaGames.thebonfire2") != string::npos && soName.find("libil2cpp.so") != string::npos && !g_on_shared_so_load_Sucess)
		{
			//_dll = handle;
			LOGE("on_shared_so_load set handle. thebonfire2\n");
			
			_curPid = find_pid_of("com.armorgames.thebonfire2");
			LOGE("_curPid: %d", _curPid);
	
			if(0 == xh_core_add_sigsegv_handler()) 
			{
				LOGE("add sigsegv handler sucess.\n");
				//init_ok = true;
			}
			else
				LOGE("add sigsegv handler failure.\n");
			
			LOGE("create address thread\n");
			_thread = std::thread(threadProc, nullptr);
			_thread.detach();
			
			g_on_shared_so_load_Sucess = true;
			LOGE("on_shared_so_load return \n");
		}
	}
	
	EXPORT_FUNC void doCheat(int arg0, int arg1, int arg2)
	{
		if (!g_on_shared_so_load_Sucess)
		{
			LOGE("doCheat do nothing just return\n");
			return;
		}
		int cheatIndex = 0;
		cheatIndex = arg0;
		LOGE("cheat index %d\n", cheatIndex);
		process_cheat_safe(arg0, arg1);
	}
}
