#include "hookStrategy.h"
#include "common.h"
#include "inlinehook\inlineHook.h"
#include "cheatSDK.h"

int (*old_GameProcessGTALCS)(float);

hookGTALCS::hookGTALCS()
{
	
}

hookGTALCS::~hookGTALCS()
{
	unHook();
}

void hookGTALCS::calFunAddr(void* dll)
{
	LOGE("hookGTALCS::calFunAddr.\n");
	char *error;
	f__ZN5CGame7ProcessEf = (int(*)(float))dlsym(dll, "_ZN5CGame7ProcessEf");
	if (f__ZN5CGame7ProcessEf)
		LOGE("find _ZN5CGame7ProcessEf.\n");
	else
		LOGE("can not find _ZN5CGame7ProcessEf.\n");
	
	if ((error = (char*)dlerror()) != NULL)  
		LOGE("dlsym _ZN5CGame7ProcessEf:	%s\n", error);
	
	Dl_info info;
	dladdr((void*)f__ZN5CGame7ProcessEf, &info);
	LOGE("f__ZN5CGame7ProcessEf Dl_info: %x	%x\n", (unsigned int)(long)(info.dli_fbase), (unsigned int)(long)(info.dli_saddr));

	funGameProcessdAddr = (long)(info.dli_saddr);
}

int hookGTALCS::replaceGameProcess(float arg)
{
	int ret = old_GameProcessGTALCS(arg);

	CCheatSDK::getInstance()->callFunByFlag();
	return ret;
}

int hookGTALCS::hookGameProcess()
{
	if (registerInlineHook((uint32_t) funGameProcessdAddr, (uint32_t) (hookGTALCS::replaceGameProcess), (uint32_t **) &old_GameProcessGTALCS) != ELE7EN_OK) 
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



int hookGTALCS::unHook()
{
	if (inlineUnHook((uint32_t) funGameProcessdAddr) != ELE7EN_OK) {
        return -1;
    }
    return 0;
}

void hookGTALCS::DoHook()
{
	hookGameProcess();
}