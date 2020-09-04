#include "hookStrategy.h"
#include "common.h"
#include "include\inlineHook.h"
#include "cheatSDK.h"

int (*old_GameProcessGTASA)();

hookGTASA::hookGTASA()
{
	
}

hookGTASA::~hookGTASA()
{
	unHook();
}

void hookGTASA::calFunAddr(void* dll)
{
	LOGE("hookGTASA::calFunAddr.\n");
	char *error;
	f__ZN5CGame7ProcessEv = (int(*)())dlsym(dll, "_ZN5CGame7ProcessEv");
	if (f__ZN5CGame7ProcessEv)
		LOGE("find _ZN5CGame7ProcessEv.\n");
	else
		LOGE("can not find _ZN5CGame7ProcessEv.\n");
	
	if ((error = (char*)dlerror()) != NULL)  
		LOGE("dlsym _ZN5CGame7ProcessEv:	%s\n", error);
	
	Dl_info info;
	dladdr((void*)f__ZN5CGame7ProcessEv, &info);
	LOGE("f__ZN5CGame7ProcessEv Dl_info: %x	%x\n", (unsigned int)(long)(info.dli_fbase), (unsigned int)(long)(info.dli_saddr));

	funGameProcessdAddr = (long)(info.dli_saddr);
}

int hookGTASA::replaceGameProcess()
{
	int ret = old_GameProcessGTASA();
	
	CCheatSDK::getInstance()->callFunByFlag();
	return ret;
}

int hookGTASA::hookGameProcess()
{
	if (registerInlineHook((uint32_t) funGameProcessdAddr, (uint32_t) (hookGTASA::replaceGameProcess), (uint32_t **) &old_GameProcessGTASA) != ELE7EN_OK) 
	{
		LOGE("registerInlineHook CGame::Process failure\n");
        return -1;
    }
	if (inlineHook((uint32_t) funGameProcessdAddr) != ELE7EN_OK) {
		LOGE("inlineHook CGame::Process failure\n");
        return -1;
    }
	LOGE("hook CGame::Process sucess\n");
	 return 0;
}



int hookGTASA::unHook()
{
	if (inlineUnHook((uint32_t) funGameProcessdAddr) != ELE7EN_OK) {
        return -1;
    }
    return 0;
}

void hookGTASA::DoHook()
{
	hookGameProcess();
}