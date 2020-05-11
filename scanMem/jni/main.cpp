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

#define LOG_PRINT
#ifdef LOG_PRINT

#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,"scanmem",__VA_ARGS__)
#define ELF_LOG(...) fprintf (stderr, __VA_ARGS__)

#else

#define  LOGE(...)
#define ELF_LOG(...)

#endif

//adb forward tcp:1111 localabstract:minitouch
//localabstract:<unix domain socket name>
//const char* g_szVersion = "1.20200509";
char g_szDefaultSocketName[64] = "android_scanmem";

int g_socket = 0;
int g_port = 1122;
pid_t g_cur_target = -1;

/********************************************************
运行elf，参数：要附加的进程，需要修改内存的地址，修改的值
./scanmemDemo2 1222 A8F79F30 99
*********************************************************/
int start_server(char* sockname)
{
	//int fd = socket(AF_UNIX, SOCK_STREAM, 0);
	int fd = socket(AF_INET, SOCK_STREAM, 0);

	if (fd < 0)
	{
		ELF_LOG("creating socket");
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
		ELF_LOG("binding socket");
		close(fd);
		return -1;
	}

	listen(fd, 1);

	return fd;
}

void process_cmd(char* io_buffer)
{
	ELF_LOG("process_cmd io_buffer:	%s\n", io_buffer);
	long int pid;
	char* nextPtr;
	void* addr;
	int data;
	char* cursor = io_buffer;
	cursor += 1;
	int status;
	switch (io_buffer[0])
	{
	case 'D':
		ELF_LOG("process_cmd D:	%s\n", io_buffer);
		pid = strtol(cursor, &cursor, 10);

		if ((g_cur_target != -1) && (g_cur_target != pid))
		{
			ELF_LOG("#########	new detach, first detach old\n");
			ptrace(PTRACE_DETACH, g_cur_target, 1, 0);
		}

		g_cur_target = (pid_t)pid;
		ELF_LOG("#########	attach\n");
		if (ptrace(PTRACE_ATTACH, g_cur_target, NULL, NULL) == -1L)
		{
			ELF_LOG("failed to attach\n");
			return;
		}
		if (waitpid(g_cur_target, &status, 0) == -1 || !WIFSTOPPED(status))
		{
			ELF_LOG("there was an error waiting for the target to stop.\n");
			return;
		}
		break;
	case 'S':
		ELF_LOG("process_cmd S:	%s\n", io_buffer);

		break;
	case 'W':
		ELF_LOG("process_cmd W:	%s\n", io_buffer);

		ELF_LOG("string split\n");
		addr = (void *)strtoll(cursor, &nextPtr, 16);
		ELF_LOG("address	%0x\n", addr);
		data = strtol(nextPtr, NULL, 10);
		ELF_LOG("value 	%d\n", data);

		ELF_LOG("#########	write mem\n");
		if (ptrace(PTRACE_POKEDATA, g_cur_target, addr, data) == -1L)
		{
			ELF_LOG("memory allocation failed.\n");
			return;
		}
		ELF_LOG("#########	detach\n");
		ptrace(PTRACE_DETACH, g_cur_target, 1, 0);
		break;
	case 'E':
		ELF_LOG("process_cmd E:	%s\n", io_buffer);

		break;
	default:
		break;
	}
}

int main(int argc, char **argv)
{
#if 0
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
#endif

	char* sockname = g_szDefaultSocketName;
	struct sockaddr_un client_addr;
	socklen_t client_addr_length = sizeof(client_addr);

	int server_fd = start_server(sockname);
	if (server_fd < 0)
	{
		ELF_LOG("start server failed:Unable to start server on %s\n", sockname);
		ELF_LOG("maybe have already start a server.\n");
		return -1;
	}
	ELF_LOG( "start server succeeded\n");
	while (1)
	{
		int client_fd = accept(server_fd, (struct sockaddr*) NULL, NULL);

		if (client_fd < 0)
		{
			ELF_LOG("accepting client exit(1)\n");
			exit(1);
		}

		ELF_LOG("Connection established\n");

		char io_buffer[80] = { 0 };
		int io_length = 0;
		char* cursor;
		long int contact, x, y, pressure;

		/*// Tell version
		io_length = snprintf(io_buffer, sizeof(io_buffer), "v %s\n", g_szVersion);
		send(client_fd, io_buffer, io_length, 0);

		// Tell help
		io_length = snprintf(io_buffer, sizeof(io_buffer), "$ %s\n", g_szHelp);
		send(client_fd, io_buffer, io_length, 0);*/

		while (1) {
			io_length = 0;

			while (io_length < sizeof(io_buffer)
					&& recv(client_fd, &io_buffer[io_length], 1, 0) == 1) {
				if (io_buffer[io_length++] == '\n') {
					break;
				}
			}

			if (io_length <= 0) {
				ELF_LOG("judgement: client %d have exited.\n", client_fd);
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

		ELF_LOG("client_fd %d Connection closed\n", client_fd);
		close(client_fd);
	}

	fprintf(stderr, "#########	return sucess\n");
	close(server_fd);

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

static jint init_native(JNIEnv *env, jobject thiz)
{
	struct sockaddr_in addr;
	g_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (g_socket < 0)
	{
		LOGE("creating socket error\n");
		return 1;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET; //设置为IP通信
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(g_port);

	if (connect(g_socket, (struct sockaddr *) &addr, sizeof(addr)) < 0)
	{
		LOGE("connect socket error %d\n", errno);
		close(g_socket);
		return 1;
	}
	LOGE("connect socket sucess %d\n", g_socket);
}

static void doCmdD_native(JNIEnv *env, jobject thiz, jint pid)
{
	LOGE("doCmdD_native %d\n", pid);
	char cmd[80] = { 0 };
	int io_length = snprintf(cmd, sizeof(cmd), "D %d\n", pid);
	send(g_socket, cmd, io_length, 0);
}

static void doCmdS_native(JNIEnv *env, jobject thiz)
{
	char cmd[80] = { 0 };
	int io_length = snprintf(cmd, sizeof(cmd), "S\n");
	send(g_socket, cmd, io_length, 0);
}

static void doCmdW_native(JNIEnv *env, jobject thiz, jstring address, jint value)
{
	const char *strAddress = env->GetStringUTFChars(address, NULL);
	void *addr;
	char *endptr;
	addr = (void *)strtoll(strAddress, &endptr, 16);

	if (getuid() != 0)
		LOGE("*** RUNNING AS unROOT. ***\n");
	else
		LOGE( "*** RUNNING AS ROOT. ***\n");

	LOGE("doCmdW_native addr %0x\n", addr);
	LOGE("doCmdW_native value %d\n", value);
	char cmd[80] = { 0 };
	int io_length = snprintf(cmd, sizeof(cmd), "W %0x %d\n", addr, value);
	LOGE("doCmdW_native cmd %s\n", cmd);
	send(g_socket, cmd, io_length, 0);

	env->ReleaseStringUTFChars(address, strAddress);
}

static void doCmdE_native(JNIEnv *env, jobject thiz)
{
	char cmd[80] = { 0 };
	int io_length = snprintf(cmd, sizeof(cmd), "E\n");
	send(g_socket, cmd, io_length, 0);
}

static void eixt_native(JNIEnv *env, jobject thiz)
{
	LOGE("socket close %d\n", g_socket);
	close(g_socket);
}

#define APP_PKG_NAME	"com/wufun91/gameModify"
# define NELEM(x) ((int) (sizeof(x) / sizeof((x)[0])))
int register_ScanMem(JNIEnv *env)
{
	static const JNINativeMethod methods[] = {
			{ "gameModifyDemo", "(Ljava/lang/String;Ljava/lang/String;I)I", (int *)gameModifyDemo_native },
			{ "init",           "()I",                                      (int *)init_native },
			{ "doCmdD",         "(I)V",                                     (void *)doCmdD_native },
			{ "doCmdS",         "()V",                                      (void *)doCmdS_native },
			{ "doCmdW",         "(Ljava/lang/String;I)V",                   (void *)doCmdW_native },
			{ "doCmdE",         "()V",                                      (void *)doCmdE_native },
			{ "exit",           "()V",                                      (void *)eixt_native },
	};

    jclass clazz;
    clazz = env->FindClass(APP_PKG_NAME"/libScanMem");
    return env->RegisterNatives(clazz, methods, NELEM(methods));
}

