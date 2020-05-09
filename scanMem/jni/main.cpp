#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <string.h>
#include <jni.h>
#include <android/log.h>

#define LOG_PRINT
#ifdef LOG_PRINT

#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,"scanmem",__VA_ARGS__)
#define ELF_LOG(...) fprintf (stderr, const char*format, [argument])

#else

#define  LOGE(...)
#define ELF_LOG(...)

#endif

/********************************************************
运行elf，参数：要附加的进程，需要修改内存的地址，修改的值
./scanmemDemo2 1222 A8F79F30 99
*********************************************************/
int main(int argc, char **argv)
{
	if (argc !=4)
	{
		fprintf(stderr, "*** argv error ***\n");
		return -1;
	}

	char sz_ProcessName[64];
	char szPid[64];
	char szAddress[64];
	char szValue[64];
	memset(sz_ProcessName, 0, 64);
	memset(szPid, 0, 64);
	memset(szAddress, 0, 64);
	memset(szValue, 0, 64);
	strcpy(sz_ProcessName,  argv[0]);
	strcpy(szPid, 			argv[1]);
	strcpy(szAddress, 		argv[2]);
	strcpy(szValue, 		argv[3]);
	fprintf(stderr, "arg[0]	process	%s\n", sz_ProcessName);
	fprintf(stderr, "arg[1]	pid     %s\n", szPid);
	fprintf(stderr, "arg[2]	address	%s\n", szAddress);
	fprintf(stderr, "arg[3]	value 	%s\n", szValue);

	if (getuid() != 0)
        fprintf(stderr, "*** RUNNING AS unROOT. ***\n");
	else
		fprintf(stderr, "*** RUNNING AS ROOT. ***\n");

	int status;
	long data = atoi(szValue);

	void *addr;
	char *endptr;
	pid_t target;
	target = (pid_t)strtoul(argv[1], &endptr, 0);
	addr = (void *)strtoll(argv[2], &endptr, 16);

	fprintf(stderr, "#########	attach\n");
	if (ptrace(PTRACE_ATTACH, target, NULL, NULL) == -1L)
	{
		fprintf(stderr, "failed to attach\n");
        return -1;
    }
	if (waitpid(target, &status, 0) == -1 || !WIFSTOPPED(status))
	{
		fprintf(stderr, "there was an error waiting for the target to stop.\n");
        return -1;
    }

	fprintf(stderr, "#########	write mem\n");
	if (ptrace(PTRACE_POKEDATA, target, addr, data) == -1L)
	{
		fprintf(stderr, "memory allocation failed.\n");
		return -1;
    }

	fprintf(stderr, "#########	detach\n");
	ptrace(PTRACE_DETACH, target, 1, 0);

	fprintf(stderr, "#########	return sucess\n");
	return 0;
}

//##################################### so #####################################

#define REGISTER_NATIVE(env, module) { \
	extern int register_##module(JNIEnv *); \
	if (register_##module(env) < 0) \
		return JNI_FALSE; \
}

static int registerNatives(JNIEnv* env)
{
	REGISTER_NATIVE(env, ScanMem);

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
void JNI_OnUnload(JavaVM* vm, void* reserved)//退出是没有被调用？？？
//退出的时候java调的System.exit(0)，不知道能不能正常调到unload函数
//JNI_OnUnload什么时候调用
{
	LOGE("JNI_OnUnload");
}

static jint gameModifyDemo_native(JNIEnv *env, jobject thiz, jstring pid, jstring address, jint value)
{
	const char *strPid = env->GetStringUTFChars(pid, NULL);
	const char *strAddress = env->GetStringUTFChars(address, NULL);

	if (getuid() != 0)
        LOGE("*** RUNNING AS unROOT. ***\n");
	else
		LOGE( "*** RUNNING AS ROOT. ***\n");

	LOGE("pid     %s\n", strPid);
	LOGE("address %s\n", strAddress);
	LOGE("value   %d\n", value);

	int status;
	long data = (long)value;

	void *addr;
	char *endptr;
	pid_t target;
	target = (pid_t)strtoul(strPid, &endptr, 0);
	addr = (void *)strtoll(strAddress, &endptr, 16);

	LOGE("#########	attach\n");
	if (ptrace(PTRACE_ATTACH, target, NULL, NULL) == -1L)
	{
		LOGE("failed to attach\n");
        return -1;
    }
	if (waitpid(target, &status, 0) == -1 || !WIFSTOPPED(status))
	{
		LOGE("there was an error waiting for the target to stop.\n");
        return -1;
    }

	LOGE("#########	write mem\n");
	if (ptrace(PTRACE_POKEDATA, target, addr, data) == -1L)
	{
		LOGE("memory allocation failed.\n");
		return -1;
    }

	LOGE( "#########	detach\n");
	ptrace(PTRACE_DETACH, target, 1, 0);

	env->ReleaseStringUTFChars(pid, strPid);
	env->ReleaseStringUTFChars(address, strAddress);
	LOGE( "#########	return sucess\n");
	return 0;
}


#define APP_PKG_NAME	"com/wufun91/gameModify"
# define NELEM(x) ((int) (sizeof(x) / sizeof((x)[0])))
int register_ScanMem(JNIEnv *env)
{
	static const JNINativeMethod methods[] = {
			{ "gameModifyDemo", "(Ljava/lang/String;Ljava/lang/String;I)I", (int *)gameModifyDemo_native },
	};

    jclass clazz;
    clazz = env->FindClass(APP_PKG_NAME"/libScanMem");
    return env->RegisterNatives(clazz, methods, NELEM(methods));
}

