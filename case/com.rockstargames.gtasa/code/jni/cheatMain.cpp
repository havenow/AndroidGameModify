#include <string.h>
#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <android/log.h>

#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,"inlinehook",__VA_ARGS__)



#include <unwind.h>
#include <dlfcn.h>
#include <vector>
#include <string>
#include <android/log.h>

/*
Application.mk中APP_STL要用gnustl_static 不然打印堆栈的代码编译不过
#APP_STL := stlport_static
APP_STL := gnustl_static
*/
static _Unwind_Reason_Code unwindCallback(struct _Unwind_Context* context, void* arg)
{
    std::vector<_Unwind_Word> &stack = *(std::vector<_Unwind_Word>*)arg;
    stack.push_back(_Unwind_GetIP(context));
    return _URC_NO_REASON;
}

void callstackDump(std::string &dump) {
    std::vector<_Unwind_Word> stack;
    _Unwind_Backtrace(unwindCallback, (void*)&stack);
    dump.append("                                                               \n"
                "*** *** *** *** *** *** *** *** *** *** *** *** *** *** *** ***\n"
                 "backtrace:\n");

    char buff[256];
    for (size_t i = 0; i < stack.size(); i++) {
        Dl_info info;
        if (!dladdr((void*)stack[i], &info)) {
            continue;
        }
        int addr = (char*)stack[i] - (char*)info.dli_fbase - 1;
        if (info.dli_sname == NULL || strlen(info.dli_sname) == 0) {
            sprintf(buff, "#%02x pc %08x  %s\n", i, addr, info.dli_fname);
        } else {
            sprintf(buff, "#%02x pc %08x  %s (%s+00)\n", i, addr, info.dli_fname, info.dli_sname);
        }
        dump.append(buff);
    }
}

void callstackLogcat(int prio, const char* tag) {
    std::string dump;
    callstackDump(dump);
    __android_log_print(prio, tag, "%s", dump.c_str());
}


void appendToLogFile(const char *format, ...)
{
	char content[1024];
	memset(content, 0, 1024);

	va_list args;
	va_start(args, format);
	vsprintf(content, format, args);
#if 1
	strcat(content, "\n");
#endif 	
	va_end(args);

	FILE* log_file = fopen("/data/local/tmp/inlineHook.log", "a+");
	if (log_file != NULL) {
		fwrite(content, strlen(content), 1, log_file);
		fflush(log_file);
		fclose(log_file);
	}
}

long funWeaponCheat1Addr;
long funGameProcessdAddr;

int (*f__ZN6CCheat12WeaponCheat1Ev)();
int (*f__ZN5CGame7ProcessEv)();

#include <dlfcn.h>    
#include <dirent.h>    
int find_pid_of(const char *process_name)    
{    
    int id;    
    pid_t pid = -1;    
    DIR* dir;    
    FILE *fp;    
    char filename[32];    
    char cmdline[256];    
    
    struct dirent * entry;    
    
    if (process_name == NULL)    
        return -1;    
    
    dir = opendir("/proc");    
    if (dir == NULL)    
        return -1;    
    
    while((entry = readdir(dir)) != NULL) {    
        id = atoi(entry->d_name);    
        if (id != 0) {    
            sprintf(filename, "/proc/%d/cmdline", id);    
            fp = fopen(filename, "r");    
            if (fp) {    
                fgets(cmdline, sizeof(cmdline), fp);    
                fclose(fp);    
    
                if (strcmp(process_name, cmdline) == 0) {    
                    /* process found */    
                    pid = id;    
                    break;    
                }    
            }    
        }    
    }    
    
    closedir(dir);    
    return pid;    
}    

#include "inlineHook.h"
//enum ele7en_status registerInlineHook(uint32_t target_addr, uint32_t new_addr, uint32_t **proto_addr)

int (*old_WeaponCheat1)(/*void* CCheatThis*/) = NULL;
int replaceWeaponCheat1(/*void* CCheatThis*/)
{
	LOGE("replaceWeaponCheat1\n");
	int ret = old_WeaponCheat1(/* CCheatThis */);
	callstackLogcat(ANDROID_LOG_ERROR, "inlinehook-callstack");
	return ret;
}

int hookWeaponCheat1()
{
	if (registerInlineHook((uint32_t) (funWeaponCheat1Addr), (uint32_t) replaceWeaponCheat1, (uint32_t **) &old_WeaponCheat1) != ELE7EN_OK) {
		LOGE("registerInlineHook failure\n");
        return -1;
    }
	if (inlineHook((uint32_t) funWeaponCheat1Addr) != ELE7EN_OK) {
		LOGE("inlineHook failure\n");
        return -1;
    }
	LOGE("hook WeaponCheat1 sucess\n");
    return 0;
}


//int __fastcall CGame::Process(CGame *this)
//_DWORD CGame::Process(CGame *__hidden this)
int (*old_GameProcess)() = NULL;
int replaceGameProcess()
{
	LOGE("replace GameProcess\n");
	int ret = old_GameProcess();
	LOGE("replace GameProcess return: %d\n", ret);
	
	f__ZN6CCheat12WeaponCheat1Ev();
	return ret;
}
int hookGameProcess()
{
	if (registerInlineHook((uint32_t) funGameProcessdAddr, (uint32_t) replaceGameProcess, (uint32_t **) &old_GameProcess) != ELE7EN_OK) 
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



int unHook()
{
	if (inlineUnHook((uint32_t) funGameProcessdAddr) != ELE7EN_OK) {
        return -1;
    }

    return 0;
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
	
	pid_t target_pid = 0; 
	target_pid = find_pid_of("com.rockstargames.gtasa");
	appendToLogFile("target_pid %d.\n", target_pid);
	LOGE("target_pid %d.\n", target_pid);
	
	void* dll = NULL;
	char *error;
	//dll = dlopen("/data/user/0/io.busniess.va/virtual/data/user/0/com.rockstargames.gtasa/app_Lib/libGTASA.so", RTLD_NOW);
	dll = dlopen("libGTASA.so", RTLD_NOW);
	if ((error = (char*)dlerror()) != NULL)  
	{
		appendToLogFile("dlsym error dlopen:	%s\n", error);
		LOGE("dlsym error dlopen:	%s\n", error);
	}
		
	else
	{
		appendToLogFile("dlsym dlopen sucess\n");
		LOGE("dlsym dlopen sucess\n");
	}
	
	if(dll==NULL)
	{
		appendToLogFile("dlsym error dll == NULL\n");
		LOGE("dlsym error dll == NULL\n");
	}
		
	else
	{
		appendToLogFile("dlsym dll != NULL\n");
		LOGE("dlsym dll != NULL\n");
	}
		
//hook CCheat::WeaponCheat1----------------------------------------------------------------------------------------------------	
	f__ZN6CCheat12WeaponCheat1Ev = (int(*)())dlsym(dll, "_ZN6CCheat12WeaponCheat1Ev");
	if (f__ZN6CCheat12WeaponCheat1Ev)
	{
		appendToLogFile("find _ZN6CCheat12WeaponCheat1Ev.\n");
		LOGE("find _ZN6CCheat12WeaponCheat1Ev.\n");
	}
	else
	{
		appendToLogFile("can not find _ZN6CCheat12WeaponCheat1Ev.\n");
		LOGE("can not find _ZN6CCheat12WeaponCheat1Ev.\n");
	}
		
	if ((error = (char*)dlerror()) != NULL)  
	{
		appendToLogFile("dlsym _ZN6CCheat12WeaponCheat1Ev:	%s\n", error);
		LOGE("dlsym _ZN6CCheat12WeaponCheat1Ev:	%s\n", error);
	}
	
	Dl_info info;
	dladdr((void*)f__ZN6CCheat12WeaponCheat1Ev, &info);
	appendToLogFile("f__ZN6CCheat12WeaponCheat1Ev          Dl_info: %x	%x\n", info.dli_fbase, info.dli_saddr);
	LOGE("f__ZN6CCheat12WeaponCheat1Ev          Dl_info: %x	%x\n", (unsigned int)(long)(info.dli_fbase), (unsigned int)(long)(info.dli_saddr));

	funWeaponCheat1Addr = (long)(info.dli_saddr);
	hookWeaponCheat1();
	
//hook CGame::Process----------------------------------------------------------------------------------------------------
	f__ZN5CGame7ProcessEv = (int(*)())dlsym(dll, "_ZN5CGame7ProcessEv");
	if (f__ZN5CGame7ProcessEv)
		LOGE("find _ZN5CGame7ProcessEv.\n");
	else
		LOGE("can not find _ZN5CGame7ProcessEv.\n");
	
	if ((error = (char*)dlerror()) != NULL)  
		LOGE("dlsym _ZN5CGame7ProcessEv:	%s\n", error);
	
	dladdr((void*)f__ZN5CGame7ProcessEv, &info);
	LOGE("f__ZN5CGame7ProcessEv Dl_info: %x	%x\n", (unsigned int)(long)(info.dli_fbase), (unsigned int)(long)(info.dli_saddr));

	funGameProcessdAddr = (long)(info.dli_saddr);
	hookGameProcess();

	result = JNI_VERSION_1_4;
	return result;
}
