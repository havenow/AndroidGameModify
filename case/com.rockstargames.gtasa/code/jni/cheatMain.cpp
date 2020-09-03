#include <string.h>
#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <map>
#include <vector>

#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>

using namespace std;

#include "common.h"
#include "cheatSDK.h"

GAME_NAME g_game = _GAME_GTAVC;
void* g_dll = NULL;
pthread_mutex_t cheat_mutex ;

char g_szDefaultSocketName[64] = "cheat";
int g_socket = 0;
int g_port = 1166;

int start_server(char* sockname)
{
	int fd = socket(AF_INET, SOCK_STREAM, 0);

	if (fd < 0)
	{
		LOGE("creating socket");
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
		LOGE("binding socket");
		close(fd);
		return -1;
	}

	listen(fd, 1);

	return fd;
}

void process_cmd(char* io_buffer)
{
	LOGE("process_cmd io_buffer:	%s\n", io_buffer);
	int index;
	char* nextPtr;
	void* addr;
	int data;
	char* cursor = io_buffer;
	cursor += 1;
	int status;
	switch (io_buffer[0])
	{
	case 'D':
		{
			char strCheatCmd[128];
			memset(strCheatCmd, '0', 128);
			sscanf(io_buffer, "D: %s", strCheatCmd);
 			pthread_mutex_lock(&cheat_mutex);
			CCheatMgr::getInstance()->setCallFunFlag(strCheatCmd);
			pthread_mutex_unlock(&cheat_mutex); 
		}
		break;
	default:
		break;
	}
}


static pthread_t cheat_thread;
void* threadProc(void* param)
{
	char* sockname = g_szDefaultSocketName;
	struct sockaddr_un client_addr;
	socklen_t client_addr_length = sizeof(client_addr);

	int server_fd = start_server(sockname);
	if (server_fd < 0)
	{
		LOGE("start server failed:Unable to start server on %s\n", sockname);
		LOGE("maybe have already start a server.\n");
		return NULL;
	}
	LOGE( "start server succeeded\n");
	while (1)
	{
		int client_fd = accept(server_fd, (struct sockaddr*) NULL, NULL);

		if (client_fd < 0)
		{
			LOGE("accepting client exit(1)\n");
			//exit(1);
			continue;
		}

		LOGE("Connection established\n");

		char io_buffer[80] = { 0 };
		int io_length = 0;
		char* cursor;
		long int contact, x, y, pressure;

		while (1) {
			io_length = 0;

			while (io_length < sizeof(io_buffer)
					&& recv(client_fd, &io_buffer[io_length], 1, 0) == 1) {
				if (io_buffer[io_length++] == '\n') {
					break;
				}
			}

			if (io_length <= 0) {
				LOGE("judgement: client %d have exited.\n", client_fd);
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

		LOGE("client_fd %d Connection closed\n", client_fd);
		close(client_fd);
	}

	close(server_fd);
	LOGE("exit cheat thread\n");
	pthread_mutex_destroy(&cheat_mutex);
    return NULL;
}


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

#include "include\inlineHook.h"
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
	//LOGE("replace GameProcess\n");
	int ret = old_GameProcess();
	//LOGE("replace GameProcess return: %d\n", ret);
	
	pthread_mutex_lock(&cheat_mutex);
	CCheatMgr::getInstance()->callFunByFlag();
	pthread_mutex_unlock(&cheat_mutex);

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
	
	char *error;
	//g_dll = dlopen("/data/user/0/io.busniess.va/virtual/data/user/0/com.rockstargames.gtasa/app_Lib/libGTASA.so", RTLD_NOW);
	switch (g_game)
	{
	case _GAME_GTASA:
		g_dll = dlopen("libGTASA.so", RTLD_NOW);
		break;
	case _GAME_GTAVC:
		g_dll = dlopen("libGTAVC.so", RTLD_NOW);
		break;
	}
	
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
	
	if(g_dll==NULL)
	{
		appendToLogFile("dlsym error dll == NULL\n");
		LOGE("dlsym error dll == NULL\n");
	}
		
	else
	{
		appendToLogFile("dlsym dll != NULL\n");
		LOGE("dlsym dll != NULL\n");
	}
	
	CCheatMgr::getInstance()->createCheatPtr(g_game);
	CCheatMgr::getInstance()->initCallMap();
	CCheatMgr::getInstance()->initCheat(g_dll);

//hook CGame::Process----------------------------------------------------------------------------------------------------
	f__ZN5CGame7ProcessEv = (int(*)())dlsym(g_dll, "_ZN5CGame7ProcessEv");
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
	hookGameProcess();

//create cheat thread-------------------------------------------------------------------------------------------------------	
	LOGE("create cheat thread.\n");
	pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_create(&cheat_thread, &attr, threadProc, NULL);
	
	pthread_mutex_init(&cheat_mutex, NULL);

	result = JNI_VERSION_1_4;
	return result;
}


int main(int argc, char** argv)
{
	struct sockaddr_in addr;
	g_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (g_socket < 0)
	{
		fprintf(stderr, "creating socket error\n");
		return 1;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET; //设置为IP通信
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(g_port);

	if (connect(g_socket, (struct sockaddr *) &addr, sizeof(addr)) < 0)
	{
		fprintf(stderr, "connect socket error %d\n", errno);
		close(g_socket);
		return 1;
	}
	fprintf(stderr, "connect socket sucess %d\n", g_socket);
	
	fprintf(stderr, "choose game:\n"
					"0:	gtasa\n"
					"1:	gtavc\n"
					">\n");
	char game[128] = {};
	scanf("%s", game);
	GAME_NAME gameIndex = GAME_NAME(atoi(game));
	CCheatMgr::getInstance()->createCheatPtr(gameIndex);
	CCheatMgr::getInstance()->initCallMap();
	CCheatMgr::getInstance()->printHelp();
		
	while (1)
	{
		fprintf(stderr, "input commonds>");
		char cmdBuf[128] = {};
		scanf("%s", cmdBuf);
		if (0 == strcmp(cmdBuf, "exit"))
		{
			fprintf(stderr, "exit cmd\n");
			break;
		}
		else if (0 == strcmp(cmdBuf, "help"))
		{
			CCheatMgr::getInstance()->printHelp();
		}
		else
		{
			int cheatIndex = 0;
			sscanf(cmdBuf, "%d", &cheatIndex);
			LOGE("cheat index %d\n", cheatIndex);
			char cmd[80] = { 0 };
			string gameFunSym = CCheatMgr::getInstance()->getFunSymByIndex(cheatIndex);
			int io_length = snprintf(cmd, sizeof(cmd), "D: %s\n", gameFunSym.c_str());
			send(g_socket, cmd, io_length, 0);
		}
	}
	
	
	fprintf(stderr, "socket close %d\n", g_socket);
	close(g_socket);
	return 0;
}