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

GAME_NAME g_game = _GAME_GTALCS;

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
			CCheatSDK::getInstance()->setCallFunFlag(strCheatCmd);
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
    return NULL;
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
	
	CCheatSDK::getInstance()->loadSo(g_game);
	
	CCheatSDK::getInstance()->chooseCheatGame(g_game);
	CCheatSDK::getInstance()->initCallMap();
	CCheatSDK::getInstance()->initCheat();
	
	CCheatSDK::getInstance()->chooseHookStrategy(g_game);
	CCheatSDK::getInstance()->setStrategyDll();
	CCheatSDK::getInstance()->DoHook();

//create cheat thread-------------------------------------------------------------------------------------------------------	
	LOGE("create cheat thread.\n");
	pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_create(&cheat_thread, &attr, threadProc, NULL);

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
					"2:	tgalcs\n"
					">\n");
	char game[128] = {};
	scanf("%s", game);
	GAME_NAME gameIndex = GAME_NAME(atoi(game));
	CCheatSDK::getInstance()->chooseCheatGame(gameIndex);
	CCheatSDK::getInstance()->initCallMap();
	CCheatSDK::getInstance()->printHelp();
		
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
			CCheatSDK::getInstance()->printHelp();
		}
		else
		{
			int cheatIndex = 0;
			sscanf(cmdBuf, "%d", &cheatIndex);
			LOGE("cheat index %d\n", cheatIndex);
			char cmd[80] = { 0 };
			string gameFunSym = CCheatSDK::getInstance()->getFunSymByIndex(cheatIndex);
			int io_length = snprintf(cmd, sizeof(cmd), "D: %s\n", gameFunSym.c_str());
			send(g_socket, cmd, io_length, 0);
		}
	}
	
	
	fprintf(stderr, "socket close %d\n", g_socket);
	close(g_socket);
	return 0;
}