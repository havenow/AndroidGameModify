#include <string.h>
#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <map>
#include <vector>

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

vector<AddrBlock> _addrBlock;
int const _deltaBlue = 0xC8;
int const _deltaSp = 0xB8;
int const _deltaAssist = 0xD8;
bool _bFindBlockDone = false;
AddrBlock _confirmBlock(nullptr, nullptr);
array<void*, 6> _addrBlood = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}; 

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
			
			unsigned long p1 = reinterpret_cast<unsigned long >(_addrBlood[0]) + 0x84;
			unsigned long p2 = reinterpret_cast<unsigned long >(_addrBlood[1]) + 0x84;
			unsigned long p3 = reinterpret_cast<unsigned long >(_addrBlood[2]) + 0x84;
			unsigned long p4 = reinterpret_cast<unsigned long >(_addrBlood[3]) + 0x84;
			unsigned long p5 = reinterpret_cast<unsigned long >(_addrBlood[4]) + 0x84;
			unsigned long p6 = reinterpret_cast<unsigned long >(_addrBlood[5]) + 0x84;
			int nP1 = *(reinterpret_cast<unsigned int*>(p1));
			int nP2 = *(reinterpret_cast<unsigned int*>(p2));
			int nP3 = *(reinterpret_cast<unsigned int*>(p3));
			int nP4 = *(reinterpret_cast<unsigned int*>(p4));
			int nP5 = *(reinterpret_cast<unsigned int*>(p5));
			int nP6 = *(reinterpret_cast<unsigned int*>(p6));
			LOGE("---dump data: %d	%d	%d	%d	%d	%d\n", nP1, nP2, nP3, nP4, nP5, nP6);
		}
		break;
	case 'E':
		{
			char strCheatCmd[128];
			int bOpen = 0;
			memset(strCheatCmd, '0', 128);
			sscanf(io_buffer, "E: %s %d", strCheatCmd, &bOpen);
			_blenchBvn.setCallFunFlag(strCheatCmd, bOpen);
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
	_timeStamp.update();
	
	if (false /*_bFindBlockDone*/)
	{
		int nCount = 0;
		array<void*, 6> addrBase;
		int index;
		for (index = 0; reinterpret_cast<unsigned long >(_confirmBlock._startAddr) + index + _deltaBlue < reinterpret_cast<unsigned long >(_confirmBlock._endAddr); index++)
		{
			unsigned long addrBlood = reinterpret_cast<unsigned long >(_confirmBlock._startAddr) + index;
			unsigned long addrSp = reinterpret_cast<unsigned long >(_confirmBlock._startAddr) + index + _deltaSp;
			unsigned long addrBlue = reinterpret_cast<unsigned long >(_confirmBlock._startAddr) + index + _deltaBlue;
			if (1083129856 == *(reinterpret_cast<unsigned int*>(addrBlood)) &&
				0 == *(reinterpret_cast<unsigned int*>(addrSp)) &&
				1079574528 == *(reinterpret_cast<unsigned int*>(addrBlue)) )
			{
				//LOGE("---updateAddr() find(contdition2): %x ~ %x ~ %x\n", _confirmBlock._startAddr+ index, _confirmBlock._startAddr + index + _deltaSp, _confirmBlock._startAddr + index + _deltaBlue);
				
				addrBase[nCount] = _confirmBlock._startAddr + index;
				nCount++;
			}
		}
		if (nCount == 2)
		{
			_addrBlood[0] = addrBase[0];
			_addrBlood[1] = addrBase[1];
			LOGE("---updateAddr() update addr(contdition2): %x   %x\n", addrBase[0], addrBase[1]);
			unsigned long p1 = reinterpret_cast<unsigned long >(addrBase[0]) + 0x70;
			unsigned long p2 = reinterpret_cast<unsigned long >(addrBase[1]) + 0x70;
			int nP1 = *(reinterpret_cast<unsigned int*>(p1));
			int nP2 = *(reinterpret_cast<unsigned int*>(p2));
			LOGE("---updateAddr() update addr(contdition2): %d   %d\n", nP1, nP2);
			_timeStampFindByBlock.update();
		}
		else if (nCount == 6)
		{
			_addrBlood[0] = addrBase[0];
			_addrBlood[1] = addrBase[1];
			_addrBlood[2] = addrBase[2];
			_addrBlood[3] = addrBase[3];
			_addrBlood[4] = addrBase[4];
			_addrBlood[5] = addrBase[5];
			LOGE("---updateAddr() update addr(contdition2): %x	%x	%x	%x	%x	%x\n", addrBase[0], addrBase[1], addrBase[2], addrBase[3], addrBase[4], addrBase[5]);
			unsigned long p1 = reinterpret_cast<unsigned long >(addrBase[0]) + 0x70;
			unsigned long p2 = reinterpret_cast<unsigned long >(addrBase[1]) + 0x70;
			unsigned long p3 = reinterpret_cast<unsigned long >(addrBase[2]) + 0x70;
			unsigned long p4 = reinterpret_cast<unsigned long >(addrBase[3]) + 0x70;
			unsigned long p5 = reinterpret_cast<unsigned long >(addrBase[4]) + 0x70;
			unsigned long p6 = reinterpret_cast<unsigned long >(addrBase[5]) + 0x70;
			int nP1 = *(reinterpret_cast<unsigned int*>(p1));
			int nP2 = *(reinterpret_cast<unsigned int*>(p2));
			int nP3 = *(reinterpret_cast<unsigned int*>(p3));
			int nP4 = *(reinterpret_cast<unsigned int*>(p4));
			int nP5 = *(reinterpret_cast<unsigned int*>(p5));
			int nP6 = *(reinterpret_cast<unsigned int*>(p6));
			LOGE("---updateAddr() update addr(contdition2): %d	%d	%d	%d	%d	%d\n", nP1, nP2, nP3, nP4, nP5, nP6);
			_timeStampFindByBlock.update();
		}
/* 		else
		{
			if (nCount)
				LOGE("---updateAddr() nCount(contdition2) %d\n", nCount);
		} */
		if (_timeStampFindByBlock.getElapsedSecond() > 90)//90s 没找到地址 需要重新确定块
		{
			_bFindBlockDone = false;
			LOGE("---_bFindBlockDone = false(contdition2): \n");
		}
		//LOGE("*************** cost time(contdition2): %lf\n", _timeStamp.getElapsedTimeInMilliSec());
		return;
	}
	
	readmaps(0);
	
	vector<AddrBlock>::iterator itor = _addrBlock.begin();
	for (itor = _addrBlock.begin(); itor != _addrBlock.end(); itor++)
	{
		AddrBlock obj = *itor;
		//LOGE("---block %x ~ %x\n", obj._startAddr, obj._endAddr);

		int nCount = 0;
		array<void*, 6> addrBase;
		int index;
		for (index = 0; reinterpret_cast<unsigned long >(obj._startAddr) + index + _deltaBlue < reinterpret_cast<unsigned long >(obj._endAddr); index++)
		{
			unsigned long addrBlood = reinterpret_cast<unsigned long >(obj._startAddr) + index;
			unsigned long addrSp = reinterpret_cast<unsigned long >(obj._startAddr) + index + _deltaSp;
			unsigned long addrBlue = reinterpret_cast<unsigned long >(obj._startAddr) + index + _deltaBlue;
			if (1083129856 == *(reinterpret_cast<unsigned int*>(addrBlood)) &&
				0 == *(reinterpret_cast<unsigned int*>(addrSp)) &&
				1079574528 == *(reinterpret_cast<unsigned int*>(addrBlue)) )
			{
				//LOGE("---updateAddr() find(contdition1): %x ~ %x ~ %x\n", obj._startAddr+ index, obj._startAddr + index + _deltaSp, obj._startAddr + index + _deltaBlue);
				
				addrBase[nCount] = _confirmBlock._startAddr + index;
				nCount++;
			}
		}
		
		if (nCount == 2)
		{
			_bFindBlockDone = true;
				
			_confirmBlock._startAddr = obj._startAddr;
			_confirmBlock._endAddr = obj._endAddr;
			LOGE("---updateAddr() confrimBblock: %x ~ %x\n", _confirmBlock._startAddr, _confirmBlock._endAddr);
				
			_addrBlood[0] = addrBase[0];
			_addrBlood[1] = addrBase[1];
			LOGE("---updateAddr() update addr(contdition1): %x   %x\n", addrBase[0], addrBase[1]);
			unsigned long p1 = reinterpret_cast<unsigned long >(addrBase[0]) + 0x70;
			unsigned long p2 = reinterpret_cast<unsigned long >(addrBase[1]) + 0x70;
			int nP1 = *(reinterpret_cast<unsigned int*>(p1));
			int nP2 = *(reinterpret_cast<unsigned int*>(p2));
			LOGE("---updateAddr() update addr(contdition1): %d   %d\n", nP1, nP2);
		}
		else if (nCount == 6)
		{
			_bFindBlockDone = true;
				
			_confirmBlock._startAddr = obj._startAddr;
			_confirmBlock._endAddr = obj._endAddr;
			LOGE("---updateAddr() confrimBblock: %x ~ %x\n", _confirmBlock._startAddr, _confirmBlock._endAddr);
			
			_addrBlood[0] = addrBase[0];
			_addrBlood[1] = addrBase[1];
			_addrBlood[2] = addrBase[2];
			_addrBlood[3] = addrBase[3];
			_addrBlood[4] = addrBase[4];
			_addrBlood[5] = addrBase[5];
			LOGE("---updateAddr() update addr(contdition1): %x	%x	%x	%x	%x	%x\n", addrBase[0], addrBase[1], addrBase[2], addrBase[3], addrBase[4], addrBase[5]);
			unsigned long p1 = reinterpret_cast<unsigned long >(addrBase[0]) + 0x70;
			unsigned long p2 = reinterpret_cast<unsigned long >(addrBase[1]) + 0x70;
			unsigned long p3 = reinterpret_cast<unsigned long >(addrBase[2]) + 0x70;
			unsigned long p4 = reinterpret_cast<unsigned long >(addrBase[3]) + 0x70;
			unsigned long p5 = reinterpret_cast<unsigned long >(addrBase[4]) + 0x70;
			unsigned long p6 = reinterpret_cast<unsigned long >(addrBase[5]) + 0x70;
			int nP1 = *(reinterpret_cast<unsigned int*>(p1));
			int nP2 = *(reinterpret_cast<unsigned int*>(p2));
			int nP3 = *(reinterpret_cast<unsigned int*>(p3));
			int nP4 = *(reinterpret_cast<unsigned int*>(p4));
			int nP5 = *(reinterpret_cast<unsigned int*>(p5));
			int nP6 = *(reinterpret_cast<unsigned int*>(p6));
			LOGE("---updateAddr() update addr(contdition1): %d	%d	%d	%d	%d	%d\n", nP1, nP2, nP3, nP4, nP5, nP6);
		}
		/* else
		{
			if (nCount)
				LOGE("---updateAddr() nCount(contdition1) %d\n", nCount);
		} */
	}
	
	//LOGE("*************** cost time(contdition1): %lf\n", _timeStamp.getElapsedTimeInMilliSec());
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

void* threadAddrProc(void* param)
{
	while(true/*!g_cheatSDKFinish*/)
	{
		_timeStamp.update();
		
		//updateAddr();
		//updateAddr_safe();
		
		if (_timeStamp.getElapsedSecond() < 2)
			usleep(2000000);
	}
	LOGE("exit calc addr thread\n");
    return NULL;
}

void* getP1BloodAddr()
{
	unsigned long p1 = reinterpret_cast<unsigned long >(_addrBlood[0]) + 0x70;
	unsigned long p2 = reinterpret_cast<unsigned long >(_addrBlood[1]) + 0x70;
	int nP1 = *(reinterpret_cast<unsigned int*>(p1));
	int nP2 = *(reinterpret_cast<unsigned int*>(p2));
	
	if (0 == nP1 && 1 == nP2)
	{
		LOGE("getP1BloodAddr: _addrBlood[0]\n");
		return _addrBlood[0];
	}
	else if (1 == nP1 && 0 == nP2)
	{
		LOGE("getP1BloodAddr: _addrBlood[1]\n");
		return _addrBlood[1];
	}
	else
	{
		LOGE("getP1BloodAddr: nullptr\n");
		return nullptr;
	}
}

int keep_blood(unsigned int p)
{
	if (0 == _curPid)
		return -1;
/* 	if (nullptr == _addrBlood[0] || nullptr == _addrBlood[1])
		return -1; */
	
	void* tmp = getP1BloodAddr();
	if (nullptr == tmp)
		return -1;
	
	unsigned int* addr = static_cast<unsigned int*>(tmp);
	*addr = 1083129856;
	return 0;
}

int keep_sp(unsigned int p)
{
	if (0 == _curPid)
		return -1;
/* 	if (nullptr == _addrBlood[0] || nullptr == _addrBlood[1])
		return -1; */
	
	void* tmp = getP1BloodAddr();
	if (nullptr == tmp)
		return -1;
	
	unsigned long addrSp = reinterpret_cast<unsigned long >(tmp) + _deltaSp;
	unsigned int* addr = reinterpret_cast<unsigned int*>(addrSp);
	*addr = 1083129856;
	return 0;
}

int keep_blue(unsigned int p)
{
	if (0 == _curPid)
		return -1;
/* 	if (nullptr == _addrBlood[0] || nullptr == _addrBlood[1])
		return -1; */
	
	void* tmp = getP1BloodAddr();
	if (nullptr == tmp)
		return -1;
	
	unsigned long addrBlue = reinterpret_cast<unsigned long >(tmp) + _deltaBlue;
	unsigned int* addr = reinterpret_cast<unsigned int*>(addrBlue);
	*addr = 1079574528;
	return 0;
}

bool _bEixtThread = false;
void* threadHookBlenchBvnProc(void* param)
{
	_timeStampFindByBlock.update();
	while(!_bEixtThread)
	{
		_timeStampHook.update();
		
		updateAddr_safe();
		
		string strCheat = "keep_blood";
		if (_blenchBvn.getFunCalllFlag(strCheat))
			keep_blood(0);
		
		string strCheat2 = "keep_sp";
		if (_blenchBvn.getFunCalllFlag(strCheat2))
			keep_sp(0);
		
		string strCheat3 = "keep_blue";
		if (_blenchBvn.getFunCalllFlag(strCheat3))
			keep_blue(0);
		
		if (_timeStampHook.getElapsedSecond() < 0.033)
			usleep(33000); 
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
	
	/*
	CCheatSDK::getInstance()->loadSo(g_game);
	
	CCheatSDK::getInstance()->chooseCheatGame(g_game);
	CCheatSDK::getInstance()->initCallMap();
	CCheatSDK::getInstance()->initCheat();
	
	CCheatSDK::getInstance()->chooseHookStrategy(g_game);
	CCheatSDK::getInstance()->setStrategyDll();
	CCheatSDK::getInstance()->DoHook();
	*/
	
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
	
//create cal addr thread-------------------------------------------------------------------------------------------------------	
/* 	LOGE("create cal addr thread.\n");
	pthread_attr_t attr1;
    pthread_attr_init(&attr1);
    pthread_attr_setdetachstate(&attr1, PTHREAD_CREATE_JOINABLE);
    pthread_create(&addr_thread, &attr1, threadAddrProc, NULL); */
	
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