#include "hookStrategy.h"
#include "common.h"
#include "inlinehook\inlineHook.h"
#include "cheatSDK.h"

int (*old_GameProcessGTA3)();

hookGTA3::hookGTA3()
{
	
}

hookGTA3::~hookGTA3()
{
	unHook();
}

void hookGTA3::calFunAddr(void* dll)
{
	LOGE("hookGTA3::calFunAddr.\n");
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

int hookGTA3::replaceGameProcess()
{
	int ret = old_GameProcessGTA3();

	CCheatSDK::getInstance()->callFunByFlag();
	return ret;
}

int hookGTA3::hookGameProcess()
{
	if (registerInlineHook((uint32_t) funGameProcessdAddr, (uint32_t) (hookGTA3::replaceGameProcess), (uint32_t **) &old_GameProcessGTA3) != ELE7EN_OK) 
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



int hookGTA3::unHook()
{
	if (inlineUnHook((uint32_t) funGameProcessdAddr) != ELE7EN_OK) {
        return -1;
    }
    return 0;
}

void hookGTA3::DoHook()
{
	hookGameProcess();
}