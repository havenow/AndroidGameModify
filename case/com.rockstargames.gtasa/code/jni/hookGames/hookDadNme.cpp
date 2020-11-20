#include "hookStrategy.h"
#include "common.h"
#include "inlinehook\inlineHook.h"
#include "cheatSDK.h"

#include <unistd.h>
#include <sys/ptrace.h> 
#include <sys/wait.h>
#include <deque>
#include <mutex>

void* (*old_DadNMe_malloc)(unsigned long);

static pthread_t dadNMe_thread;
bool hookDadNMe::_bEixtThread = false;
CTimestamp hookDadNMe::_timeStamp;
void* hookDadNMe::_bloodAddr = nullptr;
void* hookDadNMe::_crazyAddr = nullptr;
void* hookDadNMe::_alternativeBaseAddr = nullptr;
pid_t hookDadNMe::_curPid = 0;

class CheatAddr 
{
public:
	CheatAddr(void* blood, void* crazy)
	{
		_bloodAddr = blood;
		_crazyAddr = crazy;
	}
	void* _bloodAddr = nullptr;
	void* _crazyAddr = nullptr;
};

deque<CheatAddr> _addrData;
mutex _mAddr;
bool _bAddNewAddr = false;

int keep_blood()
{
	if (0 == hookDadNMe::_curPid)
		return -1;
	if (nullptr == hookDadNMe::_bloodAddr)//_bloodAddr还可以做范围检测
		return -1;
	/*pid_t target_pid = hookDadNMe::_curPid;
	int status;
	LOGE("#########	attach %d %d %d\n", hookDadNMe::_curPid, find_pid_of("com.lakegame.dadnme"), target_pid);
	if (ptrace(PTRACE_ATTACH, target_pid, NULL, NULL) == -1L)//so 打入app中，so和app在一个进程中；这样attach失败，不知道是不是写法有问题???
	//确认给了进程是给了root权限的，在MainActivity的OnCreate函数中调用了 Shell.isSuAvailable();
	//如果单独起一个进程，给root权限，或者在Virual Xposed中是可以attach的
	{
		LOGE("failed to attach\n");
		LOGE("errno %s %d\n", strerror(errno), errno);//errno Operation not permitted 1
        return -1;
    }
	if (waitpid(target_pid, &status, 0) == -1 || !WIFSTOPPED(status))
	{
		LOGE("there was an error waiting for the target to stop.\n");
        return -1;
    }
	
	char buf[] = { 0x40, 0x51, 0x80, 0x00 };
	void *data = buf;
	

	long ptraced_long = ptrace(PTRACE_PEEKDATA, target_pid, hookDadNMe::_bloodAddr, NULL);
	if (1)
	{
		LOGE("ptraced_long	%d\n", ptraced_long);
	}

	if (ptrace(PTRACE_POKEDATA, target_pid, hookDadNMe::_bloodAddr, *(long *)(data)) == -1L)	
	{
		LOGE("memory allocation failed 1.\n");
		return -1;
    }

	LOGE("#########	detach\n");
	ptrace(PTRACE_DETACH, target_pid, 1, 0);*/
	//so和app在同一个进程才能这样写
	unsigned int* addr = static_cast<unsigned int*>(hookDadNMe::_bloodAddr);
	*addr = 1079083008;//0x405180000h;1079083008D;3.2734375F
	return 0;
}

int keep_crazy()
{
	if (0 == hookDadNMe::_curPid)
		return -1;
	if (nullptr == hookDadNMe::_crazyAddr)
		return -1;
	
	unsigned int* addr = static_cast<unsigned int*>(hookDadNMe::_crazyAddr);
	*addr = 1083661312;//0x40975C00h;1079083008D;4.72998046875F
	return 0;
}

void updateAddr()
{
	if (!_bAddNewAddr)
		return;
	lock_guard<mutex> lg(_mAddr);
	//查找满足条件的地址	初始条件：血是满的、 气是空的
	bool bFindAddrSucess = false;
	deque<CheatAddr>::iterator itor;
	for (itor = _addrData.begin(); itor != _addrData.end(); *itor++)
	{
		CheatAddr tmp = *itor;
		unsigned int* addrTmp1 = static_cast<unsigned int*>(tmp._bloodAddr);
		unsigned int* addrTmp2 = static_cast<unsigned int*>(tmp._crazyAddr);
		if ((1079083008 == (*addrTmp1)) && (0 == (*addrTmp2)))
		{
			if (hookDadNMe::_bloodAddr != tmp._bloodAddr || hookDadNMe::_crazyAddr != tmp._crazyAddr)
				LOGE("+++update DadNMe _bloodAddr _crazyAddr: %x %x %d %d\n", tmp._bloodAddr, tmp._crazyAddr, 
				*(static_cast<unsigned int*>(tmp._bloodAddr)), *(static_cast<unsigned int*>(tmp._crazyAddr)));
			hookDadNMe::_bloodAddr = tmp._bloodAddr;
			hookDadNMe::_crazyAddr = tmp._crazyAddr;
			bFindAddrSucess = true;	
			break;
		}
	}


	//删除老的地址
	if (true == bFindAddrSucess)
	{
		bool bGogogo = true;
		do
		{
			if (_addrData.empty())
			{
				break;
			}
			CheatAddr reddd = _addrData.back();
			if (hookDadNMe::_bloodAddr == reddd._bloodAddr &&
				hookDadNMe::_crazyAddr == reddd._crazyAddr)
			{
				bGogogo = false;
			}
			else
				_addrData.pop_back();
		} while (bGogogo);
	}
	_bAddNewAddr = false;
}


void* hookDadNMe::threadProc(void* param)
{
	
	while(!_bEixtThread)
	{
		_timeStamp.update();
		
		updateAddr();
		
		string strCheat = "keep_blood";
		if (CCheatSDK::getInstance()->getFunCalllFlag(strCheat))
			keep_blood();
		
		string strCheat2 = "keep_crazy";
		if (CCheatSDK::getInstance()->getFunCalllFlag(strCheat2))
			keep_crazy();
		
		if (_timeStamp.getElapsedSecond() < 0.033)
			usleep(33000); 
	}
	LOGE("exit hookDadNMe thread\n");
	return NULL;
}

hookDadNMe::hookDadNMe()
{
	
}

hookDadNMe::~hookDadNMe()
{
	_bEixtThread = true;
	unHook();
}

void hookDadNMe::calFunAddr(void* dll)
{
	LOGE("hookDadNMe::calFunAddr.\n");
	char *error;
	f_malloc = (void*(*)(unsigned long))dlsym(dll, "malloc");
	if (f_malloc)
		LOGE("find malloc.\n");
	else
		LOGE("can not find malloc.\n");
	
	if ((error = (char*)dlerror()) != NULL)  
		LOGE("dlsym malloc:	%s\n", error);
	
	Dl_info info;
	dladdr((void*)f_malloc, &info);
	LOGE("f_malloc Dl_info: %x	%x\n", (unsigned int)(long)(info.dli_fbase), (unsigned int)(long)(info.dli_saddr));

	funMallocdAddr = (long)(info.dli_saddr);
}

void* hookDadNMe::replaceMalloc(unsigned long size)
{
	void* ret = old_DadNMe_malloc(size);
	int nLen = sizeof(void*);
	int nDeltaBlood32 = 0xF44;
	int nDeltaBlood64 = 0x130C;
	int nDeltaCrazy32 = 0xAA4;
	int nDeltaCrazy64 = 0xD44;
	if (8 == nLen)
	{
		if (0x1408 == size)
		{	
			LOGE("DadNMe _bloodAddr _crazyAddr: %x %x %d %d\n", ret + nDeltaBlood64, ret + nDeltaCrazy64, 
				*(static_cast<unsigned int*>(ret + nDeltaBlood64)), *(static_cast<unsigned int*>(ret + nDeltaCrazy64)));
			lock_guard<mutex> lg(_mAddr);
			CheatAddr obj(ret + nDeltaBlood64, ret + nDeltaCrazy64);
			_addrData.push_front(obj);
			_bAddNewAddr = true;
		}
	}
	else
	{
		if (0x1008 == size)
		{
			LOGE("DadNMe _bloodAddr _crazyAddr: %x %x %d %d\n", ret + nDeltaBlood32, ret + nDeltaCrazy32, 
				*(static_cast<unsigned int*>(ret + nDeltaBlood32)), *(static_cast<unsigned int*>(ret + nDeltaCrazy32)));
			lock_guard<mutex> lg(_mAddr);
			CheatAddr obj(ret + nDeltaBlood32, ret + nDeltaCrazy32);
			_addrData.push_front(obj);
			_bAddNewAddr = true;
		}
	}

	return ret;
}

int hookDadNMe::hookDadNMeMalloc()
{
	if (registerInlineHook((uint32_t) funMallocdAddr, (uint32_t) (hookDadNMe::replaceMalloc), (uint32_t **) &old_DadNMe_malloc) != ELE7EN_OK) 
	{
		LOGE("registerInlineHook CGame::Process failure\n");
        return -1;
    }
	if (inlineHook((uint32_t) funMallocdAddr) != ELE7EN_OK) {
		LOGE("inlineHook CGame::Process failure\n");
        return -1;
    }
	LOGE("hook libc.so malloc sucess\n");
	 return 0;
}



int hookDadNMe::unHook()
{
	if (inlineUnHook((uint32_t) funMallocdAddr) != ELE7EN_OK) {
        return -1;
    }
    return 0;
}

void hookDadNMe::DoHook()
{
	hookDadNMeMalloc();
	
	LOGE("create DadNMe cheat thread.\n");
	pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_create(&dadNMe_thread, &attr, hookDadNMe::threadProc, NULL);

	_curPid = find_pid_of("com.lakegame.dadnme");
}
