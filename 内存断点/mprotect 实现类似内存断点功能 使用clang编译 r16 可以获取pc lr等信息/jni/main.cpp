#include <string.h>
#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <vector>
#include <queue>
#include <mutex>
#include <thread>

#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ptrace.h>    
#include <sys/wait.h>    
#include <mutex> 
#include <assert.h>
#include <android/log.h>
#include <dlfcn.h>

char g_szDefaultSocketName[64] = "cheat";
int g_socket = 0;
int g_port = 1166;
bool g_cheatSDKFinish = false;
int g_tagetAddr = 0;

using namespace std;

#include <sys/mman.h>
#define PAGE_START(addr) ((addr) & PAGE_MASK)
#define PAGE_END(addr)   (PAGE_START(addr) + PAGE_SIZE)

#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,"inlinehook",__VA_ARGS__)

//https://www.jianshu.com/p/f445bfeea40a		Linux 信号（signal）
//https://stackoverflow.com/questions/8115192/android-ndk-getting-the-backtrace 	Android NDK: getting the backtrace
//https://blog.csdn.net/taohongtaohuyiwei/article/details/105147933
//https://github.com/alexeikh/android-ndk-backtrace-test	PrintBacktrace从这里获取的

// Android NDK r16b contains "libunwind.a" for armeabi-v7a ABI.
// This library is even silently linked in by the ndk-build,
// so we don't have to add it manually in "Android.mk".
// We can use this library, but we need matching headers,
// namely "libunwind.h" and "__libunwind_config.h".
// For NDK r16b, the headers can be fetched here:
// https://android.googlesource.com/platform/external/libunwind_llvm


 // siginfo_t {
               // int      si_signo;    /* Signal number */
               // int      si_errno;    /* An errno value */
               // int      si_code;     /* Signal code */
               // int      si_trapno;   /* Trap number that caused
                                        // hardware-generated signal
                                        // (unused on most architectures) */
               // pid_t    si_pid;      /* Sending process ID */
               // uid_t    si_uid;      /* Real user ID of sending process */
               // int      si_status;   /* Exit value or signal */
               // clock_t  si_utime;    /* User time consumed */
               // clock_t  si_stime;    /* System time consumed */
               // sigval_t si_value;    /* Signal value */
               // int      si_int;      /* POSIX.1b signal */
               // void    *si_ptr;      /* POSIX.1b signal */
               // int      si_overrun;  /* Timer overrun count; POSIX.1b timers */
               // int      si_timerid;  /* Timer ID; POSIX.1b timers */
               // void    *si_addr;     /* Memory location which caused fault */
               // int      si_band;     /* Band event */
               // int      si_fd;       /* File descriptor */
// }
#include "libunwind.h"
struct BacktraceState {
    const ucontext_t*   signal_ucontext;
    size_t              address_count = 0;
    static const size_t address_count_max = 30;
    uintptr_t           addresses[address_count_max] = {};

    BacktraceState(const ucontext_t* ucontext) : signal_ucontext(ucontext) {}

    bool AddAddress(uintptr_t ip) {
        // No more space in the storage. Fail.
        if (address_count >= address_count_max)
            return false;

        // Add the address to the storage.
        addresses[address_count++] = ip;
        return true;
    }
};

void CaptureBacktraceUsingLibUnwind(BacktraceState* state) {
    assert(state);

    // Initialize unw_context and unw_cursor.
    unw_context_t unw_context = {};
    unw_getcontext(&unw_context);
    unw_cursor_t  unw_cursor = {};
    unw_init_local(&unw_cursor, &unw_context);

    // Get more contexts.
    const ucontext_t* signal_ucontext = state->signal_ucontext;
    assert(signal_ucontext);
    const sigcontext* signal_mcontext = &(signal_ucontext->uc_mcontext);
    assert(signal_mcontext);

    // Set registers.
    unw_set_reg(&unw_cursor, UNW_ARM_R0,  signal_mcontext->arm_r0);
    unw_set_reg(&unw_cursor, UNW_ARM_R1,  signal_mcontext->arm_r1);
    unw_set_reg(&unw_cursor, UNW_ARM_R2,  signal_mcontext->arm_r2);
    unw_set_reg(&unw_cursor, UNW_ARM_R3,  signal_mcontext->arm_r3);
    unw_set_reg(&unw_cursor, UNW_ARM_R4,  signal_mcontext->arm_r4);
    unw_set_reg(&unw_cursor, UNW_ARM_R5,  signal_mcontext->arm_r5);
    unw_set_reg(&unw_cursor, UNW_ARM_R6,  signal_mcontext->arm_r6);
    unw_set_reg(&unw_cursor, UNW_ARM_R7,  signal_mcontext->arm_r7);
    unw_set_reg(&unw_cursor, UNW_ARM_R8,  signal_mcontext->arm_r8);
    unw_set_reg(&unw_cursor, UNW_ARM_R9,  signal_mcontext->arm_r9);
    unw_set_reg(&unw_cursor, UNW_ARM_R10, signal_mcontext->arm_r10);
    unw_set_reg(&unw_cursor, UNW_ARM_R11, signal_mcontext->arm_fp);
    unw_set_reg(&unw_cursor, UNW_ARM_R12, signal_mcontext->arm_ip);
    unw_set_reg(&unw_cursor, UNW_ARM_R13, signal_mcontext->arm_sp);
    unw_set_reg(&unw_cursor, UNW_ARM_R14, signal_mcontext->arm_lr);
    unw_set_reg(&unw_cursor, UNW_ARM_R15, signal_mcontext->arm_pc);

    unw_set_reg(&unw_cursor, UNW_REG_IP, signal_mcontext->arm_pc);
    unw_set_reg(&unw_cursor, UNW_REG_SP, signal_mcontext->arm_sp);

    // unw_step() does not return the first IP,
    // the address of the instruction which caused the crash.
    // Thus let's add this address manually.
    state->AddAddress(signal_mcontext->arm_pc);
	
	LOGE("ip: %x", signal_mcontext->arm_ip);
	LOGE("sp: %x", signal_mcontext->arm_sp);
	LOGE("lr: %x", signal_mcontext->arm_lr);
	LOGE("pc: %x", signal_mcontext->arm_pc);

    // Unwind frames one by one, going up the frame stack.
    while (unw_step(&unw_cursor) > 0) {
        unw_word_t ip = 0;
        unw_get_reg(&unw_cursor, UNW_REG_IP, &ip);

        bool ok = state->AddAddress(ip);
        if (!ok)
            break;
    }
}

void PrintBacktrace(BacktraceState* state) {
    assert(state);

    size_t frame_count = state->address_count;
    for (size_t frame_index = 0; frame_index < frame_count; ++frame_index) {

        void* address = (void*)(state->addresses[frame_index]);
        assert(address);

        const char* symbol_name = "";

        Dl_info info = {};
        if (dladdr(address, &info) && info.dli_sname) {
            symbol_name = info.dli_sname;
        }

        // Relative address matches the address which "nm" and "objdump"
        // utilities give you, if you compiled position-independent code
        // (-fPIC, -pie).
        // Android requires position-independent code since Android 5.0.
        unsigned long relative_address = (char*)address - (char*)info.dli_fbase;

        char* demangled = NULL;

#if ENABLE_DEMANGLING
        int status = 0;
        demangled = __cxa_demangle(symbol_name, NULL, NULL, &status);
        if (demangled)
            symbol_name = demangled;
#endif

        assert(symbol_name);
        printf("  #%02zu:  0x%lx  %s\n", frame_index, relative_address, symbol_name);
		
		/* if (info.dli_sname == NULL || strlen(info.dli_sname) == 0) {
            sprintf(buff, "#%02x pc %08x  %s\n", i, addr, info.dli_fname);
        } else {
            sprintf(buff, "#%02x pc %08x  %s (%s+00)\n", i, addr, info.dli_fname, info.dli_sname);
        } */
		
		char buff[256];
		sprintf(buff, "  #%02zu:  0x%lx  %s\n", frame_index, relative_address, symbol_name);
		LOGE("%s", buff);

        free(demangled);
    }
}

std::mutex _logMutex;
static void sigaction_usr(int sig, siginfo_t* info, void* ucontext)
{
	const ucontext_t* signal_ucontext = (const ucontext_t*)ucontext;
    assert(signal_ucontext);
	
	//common.cpp获取backtrace 是使用unwind.h Symbolization is possible via dladdr().
	
	if (sig == SIGSEGV) {
		lock_guard<mutex> lock(_logMutex);
		//LOGE("sigaction_usr SIGSEGV");
		//if (g_tagetAddr == reinterpret_cast<int>(info->si_addr))
		LOGE("signal:%d, code:%d, fault addr: %x", info->si_signo, info->si_code, info->si_addr);//打印导致SIGSEGV错误的内存地址
	
		// Android NDK r16b contains "libunwind.a" for armeabi-v7a ABI.
		BacktraceState backtrace_state(signal_ucontext);
		CaptureBacktraceUsingLibUnwind(&backtrace_state);
		
		PrintBacktrace(&backtrace_state);
	} 
}

void memWatch(int addr, int nWrite)
{
	int pagesizeConf;
	pagesizeConf = sysconf(_SC_PAGE_SIZE);
	int pageSize = PAGE_SIZE;
	int pageMask = PAGE_MASK;
	LOGE("pagesizeConf:%d, pageSize:%d, pageMask:%d\n", pagesizeConf, pageSize, pageMask);//4096 4096 -4096
	
	//uintptr_t  addr = 0x9E06D2A4;
	void* startAddr = (void *)PAGE_START(addr);
	void* endAddr = (void *)(PAGE_START(addr) + PAGE_SIZE);
	LOGE("addr: %x ~ %x ~ %x\n", startAddr, addr, endAddr);//addr: 9e06d000 ~ 9e06d2a4 ~ 9e06e000
	int ret;
	if (nWrite == 1)
		ret = mprotect((void *)PAGE_START(addr), PAGE_SIZE, PROT_READ | PROT_WRITE);
	else
		ret = mprotect((void *)PAGE_START(addr), PAGE_SIZE, PROT_READ);
	if (ret == -1)
		LOGE("mprotect fail %d\n", errno);
	else
		LOGE("mprotect sucess\n");
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
		}
		break;
	case 'E':
		{
			int targetAddr = 0;
			int nWrite = 0;
			sscanf(io_buffer, "E: %d %d", &targetAddr, &nWrite);
			g_tagetAddr = targetAddr;
			memWatch(targetAddr, nWrite);
		}
		break;
	case 'Z':
		{
			g_cheatSDKFinish = true;

		}
		break;
	default:
		break;
	}
}


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

	int ret = ::bind(fd, (struct sockaddr*) &servaddr, sizeof(servaddr));//c11 std::bind和socket ::bind函数冲突了
	if (ret < 0)
	{
		LOGE("binding socket");
		close(fd);
		return -1;
	}

	listen(fd, 1);

	return fd;
}

static pthread_t cmd_thread;
void* threadCmdProc(void* param)
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
		if (g_cheatSDKFinish)
			break;
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

	LOGE("JNI_OnLoad.\n");

	if (vm->GetEnv(&uenv.venv, JNI_VERSION_1_4) != JNI_OK) {
		return result;
	}
	env = uenv.env;
	
	if (getuid() != 0)
        LOGE("*** RUNNING AS unROOT. ***\n");
	else
		LOGE("*** RUNNING AS ROOT. ***\n");
	
	struct sigaction act;
	act.sa_flags = SA_SIGINFO;
	act.sa_sigaction = sigaction_usr;   //信号处理函数
	
	sigaction(SIGSEGV, &act, NULL);
	
	LOGE("create cmd thread.\n");
	pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_create(&cmd_thread, &attr, threadCmdProc, NULL);
	

	result = JNI_VERSION_1_4;
	return result;
}

int main(int argc, char** argv)
{
	if (getuid() != 0)
        fprintf(stderr, "*** RUNNING AS unROOT. ***\n");
	else
		fprintf(stderr, "*** RUNNING AS ROOT. ***\n");
	
	bool bForTest[2] = {false, false};
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
			;
		}
		else if (0 == strcmp(cmdBuf, "sdkFinish"))
		{
			char cmd[80] = { 0 };
			int io_length;
			io_length = snprintf(cmd, sizeof(cmd), "Z\n");
			send(g_socket, cmd, io_length, 0);
			
			fprintf(stderr, "exit cmd\n");
			break;
		}
		else
		{
			int addr = 0;
			int write = 0;
			sscanf(cmdBuf, "%d,%d", &addr, &write);
			fprintf(stderr, "data:%s,%d,%d\n", cmdBuf, addr, write);
			char cmd[80] = { 0 };
			int io_length;
			io_length = snprintf(cmd, sizeof(cmd), "E: %d %d\n", addr, write);
			fprintf(stderr, "send cmd:%s\n", cmd);
			send(g_socket, cmd, io_length, 0);
		}
	}
	
	
	fprintf(stderr, "socket close %d\n", g_socket);
	close(g_socket);
	return 0;
}
