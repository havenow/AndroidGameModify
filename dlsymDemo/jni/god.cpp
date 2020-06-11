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

extern "C"
{
__attribute__((visibility("default"))) int jniInitNativePtr(JNIEnv *env, jobject thiz, int arg0, int arg1)
{
    LOGE("#########	jniInitNativePtr begin.\n");

 
    LOGE("#########	jniInitNativePtr end.\n");
	return 0;
}

__attribute__((visibility("default"))) int jniDoProcessCheat(JNIEnv *env, jobject thiz, int arg0, int arg1, int arg2, int arg3)
{
    LOGE("#########	jniDoProcessCheat begin.\n");

   
    LOGE("#########	jniDoProcessCheat end.\n");
	return 0;
}

}



/*#define APP_PKG_NAME	"com/gameassist/plugin/nativeutils"
# define NELEM(x) ((int) (sizeof(x) / sizeof((x)[0])))
static int registerNatives(JNIEnv* env)
{
	static const JNINativeMethod methods[] = {
			{ "initNativePtr", "(II)V",(void *)jniInitNativePtr}, 
			{ "nativeProcessCheat", "(IIII)I", (int *)jniDoProcessCheat }, 
	};

    jclass clazz;
    clazz = env->FindClass(APP_PKG_NAME"/NativeUtils");
    return env->RegisterNatives(clazz, methods, NELEM(methods));
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
}*/

