#include "hookStrategy.h"
#include "common.h"
#include "inlinehook\inlineHook.h"
#include "cheatSDK.h"

#include <unistd.h>
#include <sys/ptrace.h> 
#include <sys/wait.h>

void* (*old_DadNMe_malloc)(unsigned long);

static pthread_t dadNMe_thread;
bool hookDadNMe::_bEixtThread = false;
CTimestamp hookDadNMe::_timeStamp;
void* hookDadNMe::_bloodAddr = nullptr;
void* hookDadNMe::_crazyAddr = nullptr;
void* hookDadNMe::_alternativeBaseAddr = nullptr;
pid_t hookDadNMe::_curPid = 0;

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


void* hookDadNMe::threadProc(void* param)
{
	
	while(!_bEixtThread)
	{
		_timeStamp.update();
		
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
			if (ret != _alternativeBaseAddr)
			{
				_alternativeBaseAddr = ret;
				LOGE("alternatvie DadNMe _bloodAddr _crazyAddr: %x %x\n", ret + nDeltaBlood64, ret + nDeltaCrazy64);
				unsigned int* addrTmp1 = static_cast<unsigned int*>(ret + nDeltaBlood64);
				unsigned int* addrTmp2 = static_cast<unsigned int*>(ret + nDeltaCrazy64);
				if ((0 == (*addrTmp1)) && (-13434625 == (*addrTmp2)))//ff3300ff(-13434625)
				{
					_bloodAddr = ret + nDeltaBlood64;
					_crazyAddr = ret + nDeltaCrazy64;
					LOGE("+++update DadNMe(alternative) _bloodAddr _crazyAddr\n");
				}
			}
			else
			{
				_bloodAddr = ret + nDeltaBlood64;
				_crazyAddr = ret + nDeltaCrazy64;
				LOGE("+++update DadNMe _bloodAddr _crazyAddr: %x %x\n", _bloodAddr, _crazyAddr);
			}
		}
	}
	else
	{
		if (0x1008 == size)
		{
			if (ret != _alternativeBaseAddr)
			{
				_alternativeBaseAddr = ret;
				LOGE("alternatvie DadNMe _bloodAddr _crazyAddr: %x %x %d %d\n", ret + nDeltaBlood32, ret + nDeltaCrazy32, 
				*(static_cast<unsigned int*>(ret + nDeltaBlood32)), *(static_cast<unsigned int*>(ret + nDeltaCrazy32)));
				unsigned int* addrTmp1 = static_cast<unsigned int*>(ret + nDeltaBlood32);
				unsigned int* addrTmp2 = static_cast<unsigned int*>(ret + nDeltaCrazy32);
				//LOGE("value: %d %d\n", (int)(*addrTmp1), (int)(*addrTmp2));
				if ((1065353216 == (*addrTmp1)) && (1146129076 == (*addrTmp2)))
				{
					_bloodAddr = ret + nDeltaBlood32;
					_crazyAddr = ret + nDeltaCrazy32;
					LOGE("+++update DadNMe(alternative) _bloodAddr _crazyAddr\n");
				}
			}
			else//连续两次出现时
			{
				//1065353216 1146129076
				_bloodAddr = ret + nDeltaBlood32;
				_crazyAddr = ret + nDeltaCrazy32;
				LOGE("+++update DadNMe _bloodAddr _crazyAddr: %x %x %d %d\n", _bloodAddr, _crazyAddr, 
				*(static_cast<unsigned int*>(_bloodAddr)), *(static_cast<unsigned int*>(_crazyAddr)));
				/*unsigned int* addrTmp1 = static_cast<unsigned int*>(_bloodAddr);
				unsigned int* addrTmp2 = static_cast<unsigned int*>(_crazyAddr);
				LOGE("value: %d %d\n", (int)(*addrTmp1), (int)(*addrTmp2));*/
			}
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
