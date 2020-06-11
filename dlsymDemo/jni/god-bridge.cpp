#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>

#include <string.h>
#include <jni.h>
#include <android/log.h>

#include <dlfcn.h>


#include "common.h"

#include <stdarg.h>
void appendToLogFile(const char *format, ...)
{
	char content[1024];
	memset(content, 0, 1024);

	va_list args;
	va_start(args, format);
	vsprintf(content, format, args);
#if 1	//Ìí¼Ó»»ÐÐ
	strcat(content, "\n");
#endif 	
	va_end(args);

	FILE* log_file = fopen("/data/local/tmp/libgod.log", "a+");
	if (log_file != NULL) {
		fwrite(content, strlen(content), 1, log_file);
		fflush(log_file);
		fclose(log_file);
	}
}

//static void(*f_initNativePtr)(int arg0, int arg1);
//static int(*f_nativeProcessCheat)(int arg0, int arg1, int arg2, int arg3);

//int (*f_initNativePtr)(JNIEnv *env, jobject thiz, unsigned int a3, unsigned int a4);
//int (*f_nativeProcessCheat)(JNIEnv *env, jobject thiz, int a3, int a4, int a5, int a6);

int (*f_initNativePtr)(JNIEnv *env, jobject thiz, unsigned int a3, unsigned int a4);
int (*f_nativeProcessCheat)(JNIEnv *env, jobject thiz, int a3, int a4, int a5, int a6);
jint (*f_JNIOnLoad)(JavaVM* vm, void* reserved);

JavaVM* g_vm = NULL;
void* g_reserved = NULL;

#define REGISTER_NATIVE(env, module) { \
	extern int register_##module(JNIEnv *); \
	if (register_##module(env) < 0) \
		return JNI_FALSE; \
}

static int registerNatives(JNIEnv* env)
{
	REGISTER_NATIVE(env, GodBridge);

	return JNI_TRUE;
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

	LOGE("JNI_OnLoad");
	g_vm = vm;
	g_reserved = reserved;
	if (vm->GetEnv(&uenv.venv, JNI_VERSION_1_4) != JNI_OK) {
		LOGE("ERROR: GetEnv failed");
		goto bail;
	}
	env = uenv.env;

	if (registerNatives(env) != JNI_TRUE) {
		LOGE("ERROR: registerNatives failed");
		goto bail;
	}

	result = JNI_VERSION_1_4;

bail:
	return result;
}

__attribute__((visibility("default")))
void JNI_OnUnload(JavaVM* vm, void* reserved)
{
	LOGE("JNI_OnUnload");
}

static void init_bridge(JNIEnv *env, jobject thiz, jint arg0)
{
	appendToLogFile("#########	init_bridge begin.\n");
	
	f_initNativePtr = NULL;
	f_nativeProcessCheat = NULL;
	f_JNIOnLoad = NULL;

	void* dll = NULL;
	char *error;
	if (arg0 == 1)
		dll = dlopen("libpgod-ori.so", RTLD_NOW);
	else
		dll = dlopen("libpgod-test.so", RTLD_NOW);
	if ((error = (char*)dlerror()) != NULL)  
		appendToLogFile("dlsym error dlopen:	%s\n", error);
	else
		appendToLogFile("dlsym dlopen sucess\n");
	
	if(dll==NULL)
		appendToLogFile("dlsym error dll == NULL\n");
	else
		appendToLogFile("dlsym dll != NULL\n");
	
	f_JNIOnLoad = (jint(*)(JavaVM*, void*))dlsym(dll, "JNI_OnLoad");
	if (f_JNIOnLoad)
		appendToLogFile("find f_JNIOnLoad.\n");
	else
		appendToLogFile("can not find f_JNIOnLoad.\n");
	if ((error = (char*)dlerror()) != NULL)  
		appendToLogFile("dlsym f_JNIOnLoad:	%s\n", error);
	
	if ((g_vm != NULL) && (f_JNIOnLoad != NULL))
	{
		//void reserved;
		f_JNIOnLoad(g_vm, g_reserved);
		appendToLogFile("call f_JNIOnLoad.\n");
	}
		
	//f_initNativePtr = (int(*)(JNIEnv *, jobject, unsigned int, unsigned int ))dlsym(dll, "Java_com_gameassist_plugin_nativeutils_NativeUtils_initNativePtr");
	f_initNativePtr = (int(*)(JNIEnv *, jobject, unsigned int, unsigned int ))dlsym(dll, "jniInitNativePtr");
	if (f_initNativePtr)
		appendToLogFile("find jniInitNativePtr.\n");
	else
		appendToLogFile("can not find jniInitNativePtr.\n");
	if ((error = (char*)dlerror()) != NULL)  
		appendToLogFile("dlsym f_initNativePtr:	%s\n", error);
	
	f_nativeProcessCheat = (int(*)(JNIEnv *, jobject, int, int, int, int))dlsym(dll, "jniDoProcessCheat");
	if (f_nativeProcessCheat)
		appendToLogFile("find jniDoProcessCheat.\n");
	else
		appendToLogFile("can not find jniDoProcessCheat.\n");
	if ((error = (char*)dlerror()) != NULL)  
		appendToLogFile("dlsym f_nativeProcessCheat:	%s\n", error);
	
	appendToLogFile("#########	init_bridge end.\n");
}

static void initNativePtr_bridge(JNIEnv *env, jobject thiz, jint arg0, jint arg1)
{
    appendToLogFile("#########	initNativePtr_bridge begin.\n");
	
	appendToLogFile("call initNativePtr args: %d %d\n", arg0, arg1);
	if (f_initNativePtr)
		f_initNativePtr(env, thiz, arg0, arg1);
	else
		appendToLogFile("f_initNativePtr is null\n");
 
    appendToLogFile("#########	initNativePtr_bridge end.\n");
}

static int nativeProcessCheat_bridge(JNIEnv *env, jobject thiz, jint arg0, jint arg1, jint arg2, jint arg3)
{
    appendToLogFile("#########	nativeProcessCheat_bridge begin.\n");

	appendToLogFile("call nativeProcessCheat args: %d %d\n", arg0, arg1);
	if (f_nativeProcessCheat)
		f_nativeProcessCheat(env, thiz, arg0, arg1, arg2, arg3);
	else
		appendToLogFile("f_nativeProcessCheat is null\n");
   
    appendToLogFile("#########	nativeProcessCheat_bridge end.\n");
}

#define APP_PKG_NAME	"com/gameassist/plugin/nativeutils"
# define NELEM(x) ((int) (sizeof(x) / sizeof((x)[0])))
int register_GodBridge(JNIEnv *env)
{
	static const JNINativeMethod methods[] = {
			{ "init", "(I)V",(void *)init_bridge}, 
			{ "initNativePtr", "(II)V", (void *)initNativePtr_bridge }, 
			{ "nativeProcessCheat", "(IIII)I", (int *)nativeProcessCheat_bridge }
	};

    jclass clazz;
    clazz = env->FindClass(APP_PKG_NAME"/NativeUtils");
    return env->RegisterNatives(clazz, methods, NELEM(methods));
}
