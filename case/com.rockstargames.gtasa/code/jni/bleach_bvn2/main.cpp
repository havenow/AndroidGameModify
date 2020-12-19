#include <string.h>
#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <map>
#include <vector>
#include <queue>
#include <mutex>

#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>

using namespace std;

#include "common.h"
#include "cheatBleachBvn.h"

static CCheatBlenchBvn _blenchBvn;
pid_t _curPid = 0;

char g_szDefaultSocketName[64] = "cheat";
int g_socket = 0;
int g_port = 1166;
bool g_cheatSDKFinish = false;
bool g_findAddrBase = false;

class AddrBlock 
{
public:
	AddrBlock(void* start, void* end)
	{
		_startAddr = start;
		_endAddr = end;
	}
	void* _startAddr = nullptr;
	void* _endAddr = nullptr;
};

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

vector<AddrBlock> _addrBlock;
array<void*, 6> _addrBase = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}; 
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

void process_cheat(int arg0, int arg1)
{
	if (0 == _curPid)
		return;

	if (nullptr == _addrBase[0])
	{
		lock_guard<mutex> lock(_cheatCacheMutex);
		CheatCache obj(arg0, arg1);
		_cheatCache.push(obj);
		return;
	}
	
	unsigned long addrTmp = 0;
	int value = 1;
	switch (arg0)
	{
	case 1://keep_blood
		addrTmp = reinterpret_cast<unsigned long >(_addrBase[0]) + 0xC;
		if (arg1 != 1)
			value = 20;
		break;
	case 2://keep_sp
		addrTmp = reinterpret_cast<unsigned long >(_addrBase[0]) + 0x10;
		if (arg1 != 1)
			value = 21;
		break;
	case 3://keep_blue
		addrTmp = reinterpret_cast<unsigned long >(_addrBase[0]) + 0x14;
		if (arg1 != 1)
			value = 22;
		break;		
	case 4://keep_assist
		addrTmp = reinterpret_cast<unsigned long >(_addrBase[0]) + 0x18;
		if (arg1 != 1)
			value = 23;
		break;		
	}
	unsigned int* addr = reinterpret_cast<unsigned int*>(addrTmp);
	*addr = value;
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
			//_blenchBvn.setCallFunFlag(strCheatCmd);
			
		}
		break;
	case 'E':
		{
			char strCheatCmd[128];
			int bOpen = 0;
			memset(strCheatCmd, '0', 128);
			sscanf(io_buffer, "E: %s %d", strCheatCmd, &bOpen);
			_blenchBvn.setCallFunFlag(strCheatCmd, bOpen);
			
			int index = 0;
			if (strcmp("keep_blood", strCheatCmd) == 0){
				index = 1;
			} else if (strcmp("keep_sp", strCheatCmd) == 0){
				index = 2;
			} else if (strcmp("keep_blue", strCheatCmd) == 0){
				index = 3;
			} else if (strcmp("keep_assist", strCheatCmd) == 0){
				index = 4;
			} 
			process_cheat(index, bOpen);
		}
		break;
	case 'Z':
		{
			g_cheatSDKFinish = true;
			//CCheatSDK::getInstance()->cheatSDKFinish();
		}
		break;
	default:
		break;
	}
}


static pthread_t cheat_thread;
void* threadProc(void* param)
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

#include "timestamp.hpp"
CTimestamp _timeStamp;
CTimestamp _timeStampHook;
CTimestamp _timeStampFindByBlock;
static pthread_t addr_thread;
static pthread_t hook_thread;

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
	_addrBlock.clear();
	
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
			if (read == 'r' && write == 'w' && exec == '-' && cow == 'p' && strcmp(filename, "") == 0)
			{
				 //LOGE("---find %x ~ %x\n", start, end);
				 //anonymnousMemSize += end - start;
				 AddrBlock obj(reinterpret_cast<void*>(start), reinterpret_cast<void*>(end));
				 _addrBlock.push_back(obj);
			}
        }
			
    }

	vector<AddrBlock>::iterator itor = _addrBlock.begin();
	for (itor = _addrBlock.begin(); itor != _addrBlock.end(); itor++)
	{
		AddrBlock obj = *itor;
		//LOGE("---find %x ~ %x\n", obj._startAddr, obj._endAddr);
		anonymnousMemSize += reinterpret_cast<unsigned long >(obj._endAddr) - reinterpret_cast<unsigned long >(obj._startAddr);
	}
	 //LOGE("!!!anonymnousMemSize %x\n", anonymnousMemSize);
    /* release memory allocated */
    free(line);
    fclose(maps);

    return true;
}

void updateAddr()
{
	//_timeStamp.update();
	if (g_findAddrBase)
		return;
	
	readmaps(0);
	
	vector<AddrBlock>::iterator itor = _addrBlock.begin();
	for (itor = _addrBlock.begin(); itor != _addrBlock.end(); itor++)
	{
		AddrBlock obj = *itor;
		LOGE("---block %x ~ %x\n", obj._startAddr, obj._endAddr);

		int nCount = 0;
		array<void*, 6> addrBase;
		int index;
		for (index = 0; reinterpret_cast<unsigned long >(obj._startAddr) + index + 0x1C < reinterpret_cast<unsigned long >(obj._endAddr); index++)
		{
			unsigned long addr1 		= reinterpret_cast<unsigned long>(obj._startAddr) + index;
			unsigned long addr2 		= reinterpret_cast<unsigned long>(obj._startAddr) + index + 0x4;
			unsigned long addr3 		= reinterpret_cast<unsigned long>(obj._startAddr) + index + 0x8;
			unsigned long addrBlood 	= reinterpret_cast<unsigned long>(obj._startAddr) + index + 0xC;
			unsigned long addrSp 		= reinterpret_cast<unsigned long>(obj._startAddr) + index + 0x10;
			unsigned long addrBlue 		= reinterpret_cast<unsigned long>(obj._startAddr) + index + 0x14;
			unsigned long addrAssist 	= reinterpret_cast<unsigned long>(obj._startAddr) + index + 0x18;
			unsigned long addr8 		= reinterpret_cast<unsigned long>(obj._startAddr) + index + 0x1C;
			if (10 == *(reinterpret_cast<unsigned int*>(addr1)) 		&&
				11 == *(reinterpret_cast<unsigned int*>(addr2)) 		&&
				12 == *(reinterpret_cast<unsigned int*>(addr3)) 		&&
				20 == *(reinterpret_cast<unsigned int*>(addrBlood)) 	&&
				21 == *(reinterpret_cast<unsigned int*>(addrSp)) 		&&
				22 == *(reinterpret_cast<unsigned int*>(addrBlue)) 		&&
				30 == *(reinterpret_cast<unsigned int*>(addrAssist)) 	&&
				40 == *(reinterpret_cast<unsigned int*>(addr8)))
			{
				//LOGE("---updateAddr() find: %x\n", obj._startAddr+ index);
				
				unsigned long addrTmp =  reinterpret_cast<unsigned long >(obj._startAddr) + index;
				addrBase[nCount] = reinterpret_cast<void*>(addrTmp);//obj._startAddr + index;
				nCount++;
			}
		}
		
		if (nCount == 1)
		{			
			_addrBase[0] = addrBase[0];
			LOGE("---updateAddr() update addr: %x\n", addrBase[0]);
			g_findAddrBase = true;
		}
		else
		{
			if (nCount)
				LOGE("---updateAddr() nCount %d\n", nCount);
		} 
	}
	
	//LOGE("*************** cost time: %lf\n", _timeStamp.getElapsedTimeInMilliSec());
}

void updateAddr_safe()
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
}

int keep_blood(int value)
{
	if (0 == _curPid)
		return -1;
 	if (nullptr == _addrBase[0])
		return -1; 
	
	void* tmp = _addrBase[0];
	if (nullptr == tmp)
		return -1;
	
	unsigned long addrTmp = reinterpret_cast<unsigned long >(tmp) + 0xC;
	unsigned int* addr = reinterpret_cast<unsigned int*>(addrTmp);
	*addr = value;
	return 0;
}

int keep_sp(int value)
{
	if (0 == _curPid)
		return -1;
	if (nullptr == _addrBase[0])
		return -1; 
	
	void* tmp = _addrBase[0];
	if (nullptr == tmp)
		return -1;
	
	unsigned long addrTmp = reinterpret_cast<unsigned long >(tmp) + 0x10;
	unsigned int* addr = reinterpret_cast<unsigned int*>(addrTmp);
	*addr = value;
	return 0;
}

int keep_blue(int value)
{
	if (0 == _curPid)
		return -1;
	if (nullptr == _addrBase[0])
		return -1; 
	
	void* tmp = _addrBase[0];
	if (nullptr == tmp)
		return -1;
	
	unsigned long addrTmp = reinterpret_cast<unsigned long >(tmp) + 0x14;
	unsigned int* addr = reinterpret_cast<unsigned int*>(addrTmp);
	*addr = value;
	return 0;
}

int keep_assist(int value)
{
	if (0 == _curPid)
		return -1;

	if (nullptr == _addrBase[0])
		return -1; 
	
	void* tmp = _addrBase[0];
	if (nullptr == tmp)
		return -1;
	
	unsigned long addrTmp = reinterpret_cast<unsigned long >(tmp) + 0x18;
	unsigned int* addr = reinterpret_cast<unsigned int*>(addrTmp);
	*addr = value;
	return 0;
}

void* threadHookBlenchBvnProc(void* param)
{
	_timeStampFindByBlock.update();
	while(!g_findAddrBase)
	{
		_timeStampHook.update();
		
		updateAddr_safe();
		
		if (_timeStampHook.getElapsedSecond() < 0.25)
			usleep(250000); 
	}
	if (nullptr != _addrBase[0])
	{
		lock_guard<mutex> lock(_cheatCacheMutex);
		while (!_cheatCache.empty())
		{
			CheatCache tmp = _cheatCache.front();
			process_cheat(tmp._index, tmp._open);
			_cheatCache.pop();
		}
	}
	LOGE("exit hookBlenchBvn thread\n");
	return NULL;
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
	
	g_cheatSDKFinish = false;
	
	_curPid = find_pid_of("com.jarworld.bleach.bvn");
	
	if(0 == xh_core_add_sigsegv_handler()) 
	{
		LOGE("add sigsegv handler sucess.\n");
		//init_ok = true;
	}
	else
		LOGE("add sigsegv handler failure.\n");
	
	_blenchBvn.initCallMap();
	_blenchBvn.initCheat();
	
//create cheat thread-------------------------------------------------------------------------------------------------------	
	LOGE("create cheat thread.\n");
	pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_create(&cheat_thread, &attr, threadProc, NULL);
	
//create hook thread-------------------------------------------------------------------------------------------------------	
	LOGE("create bleach bvn hook thread.\n");
	pthread_attr_t attr2;
    pthread_attr_init(&attr2);
    pthread_attr_setdetachstate(&attr2, PTHREAD_CREATE_JOINABLE);
    pthread_create(&hook_thread, &attr2, threadHookBlenchBvnProc, NULL);
	
	result = JNI_VERSION_1_4;
	return result;
}


int main(int argc, char** argv)
{
	if (getuid() != 0)
        fprintf(stderr, "*** RUNNING AS unROOT. ***\n");
	else
		fprintf(stderr, "*** RUNNING AS ROOT. ***\n");
	
	bool bForTest[3] = {false, false, false};
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
	
	/*fprintf(stderr, "choose game:\n"
					"0:	gtasa\n"
					"1:	gtavc\n"
					"2:	gtalcs\n"
					"3:	gta3\n"
					"4:	dadnme\n"
					"5:	blench_bvn\n"
					">\n");
	char game[128] = {};
	scanf("%s", game);*/
	/*GAME_NAME gameIndex = GAME_NAME(atoi(game));
	CCheatSDK::getInstance()->chooseCheatGame(gameIndex);
	CCheatSDK::getInstance()->initCallMap();
	CCheatSDK::getInstance()->printHelp();*/
		
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
			fprintf(stderr, "choose cheat:\n"
					"0:	keep_blood\n"
					"1:	keep_sp\n"
					"2:	keep_blue\n"
					"3:	keep_assist\n"
					">\n");
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
			int cheatIndex = 0;
			sscanf(cmdBuf, "%d", &cheatIndex);
			LOGE("cheat index %d\n", cheatIndex);
			char cmd[80] = { 0 };
			string gameFunSym ;//= CCheatSDK::getInstance()->getFunSymByIndex(cheatIndex);
			if (0 == cheatIndex){
				gameFunSym = "keep_blood";
			} else if (1 == cheatIndex){
				gameFunSym = "keep_sp";
			} else if (2 == cheatIndex){
				gameFunSym = "keep_blue";
			} else if (3 == cheatIndex){
				gameFunSym = "keep_assist";
			} else if ( 9 == cheatIndex){
				gameFunSym = "dump_mem";
			}
			int io_length;
			if (1/*gameIndex == blench_bvn*/)
			{
				if (9 == cheatIndex)
				{
					io_length = snprintf(cmd, sizeof(cmd), "D: %s\n", gameFunSym.c_str());
				}
				else
				{
					io_length = snprintf(cmd, sizeof(cmd), "E: %s %d\n", gameFunSym.c_str(), !bForTest[cheatIndex]);
					bForTest[cheatIndex] = !bForTest[cheatIndex];
				}
			}
			else
			{
				io_length = snprintf(cmd, sizeof(cmd), "D: %s\n", gameFunSym.c_str());
			}
			send(g_socket, cmd, io_length, 0);
		}
	}
	
	
	fprintf(stderr, "socket close %d\n", g_socket);
	close(g_socket);
	return 0;
}