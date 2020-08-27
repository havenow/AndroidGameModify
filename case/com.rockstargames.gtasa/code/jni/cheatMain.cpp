#include <string.h>
#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <android/log.h>
#include <pthread.h>
#include <map>
#include <vector>
#include <dlfcn.h>

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

#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,"inlinehook",__VA_ARGS__)

void* g_dllGTASA = NULL;
#ifndef _TAG_CHEAT_UNIT_
#define _TAG_CHEAT_UNIT_
typedef struct tagCheatUnit
{
	char strCheatName[128];
	int (*pfunCheat)();
	bool bCall;
} CheatUnit;
#endif

pthread_mutex_t cheat_mutex ;
map<string, CheatUnit> g_cheatData;
vector <string> cheatNameData = {
	"_ZN6CCheat12WeaponCheat1Ev",
	"_ZN6CCheat12WeaponCheat2Ev",
	"_ZN6CCheat12WeaponCheat3Ev",
	"_ZN6CCheat12WeaponCheat4Ev",
	"_ZN6CCheat15TimeTravelCheatEv",
	"_ZN6CCheat17ScriptBypassCheatEv",
	"_ZN6CCheat17ShowMappingsCheatEv",
	"_ZN6CCheat25TogglePlayerInvincibilityEv",
	"_ZN6CCheat21ToggleShowTapToTargetEv",
	"_ZN6CCheat19ToggleShowTargetingEv",
	"_ZN6CCheat22MoneyArmourHealthCheatEv",
	"_ZN6CCheat18WantedLevelUpCheatEv",
	"_ZN6CCheat20WantedLevelDownCheatEv",
	"_ZN6CCheat22ExtraSunnyWeatherCheatEv",
	"_ZN6CCheat18CloudyWeatherCheatEv",
	"_ZN6CCheat17RainyWeatherCheatEv",
	"_ZN6CCheat17FoggyWeatherCheatEv",
	"_ZN6CCheat13FastTimeCheatEv",
	"_ZN6CCheat13SlowTimeCheatEv",
	"_ZN6CCheat11MayhemCheatEv",
	"_ZN6CCheat27EverybodyAttacksPlayerCheatEv",
	"_ZN6CCheat9TankCheatEv",
	"_ZN6CCheat13StockCarCheatEv",
	"_ZN6CCheat14StockCar2CheatEv",
	"_ZN6CCheat14StockCar3CheatEv",
	"_ZN6CCheat14StockCar4CheatEv",
	"_ZN6CCheat11HearseCheatEv",
	"_ZN6CCheat13LovefistCheatEv",
	"_ZN6CCheat16TrashmasterCheatEv",
	"_ZN6CCheat13GolfcartCheatEv",
	"_ZN6CCheat15BlowUpCarsCheatEv",
	"_ZN6CCheat12SuicideCheatEv",
	"_ZN6CCheat13PinkCarsCheatEv",
	"_ZN6CCheat14BlackCarsCheatEv",
	"_ZN6CCheat8FatCheatEv",
	"_ZN6CCheat11MuscleCheatEv",
	"_ZN6CCheat15TheGamblerCheatEv",
	"_ZN6CCheat11SkinnyCheatEv",
	"_ZN6CCheat15ElvisLivesCheatEv",
	"_ZN6CCheat18VillagePeopleCheatEv",
	"_ZN6CCheat15BeachPartyCheatEv",
	"_ZN6CCheat10GangsCheatEv",
	"_ZN6CCheat13GangLandCheatEv",
	"_ZN6CCheat10NinjaCheatEv",
	"_ZN6CCheat20LoveConquersAllCheatEv",
	"_ZN6CCheat19AllCarsAreShitCheatEv",
	"_ZN6CCheat20AllCarsAreGreatCheatEv",
	"_ZN6CCheat11FlyboyCheatEv",
	"_ZN6CCheat11VortexCheatEv",
	"_ZN6CCheat13MidnightCheatEv",
	"_ZN6CCheat9DuskCheatEv",
	"_ZN6CCheat10StormCheatEv",
	"_ZN6CCheat14SandstormCheatEv",
	"_ZN6CCheat13PredatorCheatEv",
	"_ZN6CCheat14ParachuteCheatEv",
	"_ZN6CCheat12JetpackCheatEv",
	"_ZN6CCheat14NotWantedCheatEv",
	"_ZN6CCheat11WantedCheatEv",
	"_ZN6CCheat9RiotCheatEv",
	"_ZN6CCheat13FunhouseCheatEv",
	"_ZN6CCheat15AdrenalineCheatEv",
	"_ZN6CCheat12DrivebyCheatEv",
	"_ZN6CCheat24CountrysideInvasionCheatEv",
	"_ZN6CCheat12StaminaCheatEv",
	"_ZN6CCheat17WeaponSkillsCheatEv",
	"_ZN6CCheat18VehicleSkillsCheatEv",
	"_ZN6CCheat11ApacheCheatEv",
	"_ZN6CCheat9QuadCheatEv",
	"_ZN6CCheat11TankerCheatEv",
	"_ZN6CCheat10DozerCheatEv",
	"_ZN6CCheat15StuntPlaneCheatEv",
	"_ZN6CCheat17MonsterTruckCheatEv",
	"_ZN6CCheat15xboxHelperCheatEv"
};
void initCheat()
{
	for (auto elem : cheatNameData)
	{
		string cheat = elem;

		CheatUnit obj;
		strcpy(obj.strCheatName, cheat.c_str());
		if (g_dllGTASA)
		{
			obj.pfunCheat = (int(*)())dlsym(g_dllGTASA, obj.strCheatName);
			if (obj.pfunCheat)
				LOGE("find %s.\n", obj.strCheatName);
			else
				LOGE("can not find %s.\n", obj.strCheatName);
			char *error;	
			if ((error = (char*)dlerror()) != NULL)  
				LOGE("dlsym %s:	%s\n", obj.strCheatName, error);
		}
		else
			obj.pfunCheat = nullptr;

		obj.bCall = false;
		g_cheatData[cheat] = obj;
	}
}

char g_szDefaultSocketName[64] = "cheat";
int g_socket = 0;
int g_port = 1166;
bool g_bCallWeaponCheat1 = false;

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
			auto pItem = g_cheatData.find(strCheatCmd);
			if (pItem != g_cheatData.end())
			{
				(*pItem).second.bCall = true;
				LOGE("process_cmd: %s", (*pItem).second.strCheatName);
			}
			else
				LOGE("cant find process_cmd: %s", strCheatCmd); 
			pthread_mutex_unlock(&cheat_mutex); 
/* 			pthread_mutex_lock(&cheat_mutex);
			for (auto elem : g_cheatData)
			{
				if (0 == strcmp(elem.second.strCheatName, strCheatCmd))
				{
					elem.second.bCall = true;	//没有赋值给g_cheatData elem应该是引用
					LOGE("process_cmd: %s", elem.second.strCheatName);
					break;
				}
			}
			pthread_mutex_unlock(&cheat_mutex); */
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
	//LOGE("replace GameProcess\n");
	int ret = old_GameProcess();
	//LOGE("replace GameProcess return: %d\n", ret);
	
	pthread_mutex_lock(&cheat_mutex);
	/* for (auto elem : g_cheatData)
	{
		if (true == elem.second.bCall)
		{
			LOGE("replaceGameProcess %s %d\n", elem.second.strCheatName, elem.second.bCall);
			(elem.second.pfunCheat)();
			elem.second.bCall = false;
			LOGE("replaceGameProcess %s %d\n", elem.second.strCheatName, elem.second.bCall);
		}
		
	} */
	std::map<string, CheatUnit>::iterator item;
	for (item = g_cheatData.begin(); item != g_cheatData.end(); item++)
	{
		if (item->second.bCall)
		{
			LOGE("replaceGameProcess %s %d\n", item->second.strCheatName, item->second.bCall);
			(item->second.pfunCheat)();
			item->second.bCall = false;
		}
	}
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
	//g_dllGTASA = dlopen("/data/user/0/io.busniess.va/virtual/data/user/0/com.rockstargames.gtasa/app_Lib/libGTASA.so", RTLD_NOW);
	g_dllGTASA = dlopen("libGTASA.so", RTLD_NOW);
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
	
	if(g_dllGTASA==NULL)
	{
		appendToLogFile("dlsym error dll == NULL\n");
		LOGE("dlsym error dll == NULL\n");
	}
		
	else
	{
		appendToLogFile("dlsym dll != NULL\n");
		LOGE("dlsym dll != NULL\n");
	}
	
	initCheat();
		
//hook CCheat::WeaponCheat1----------------------------------------------------------------------------------------------------	
	f__ZN6CCheat12WeaponCheat1Ev = (int(*)())dlsym(g_dllGTASA, "_ZN6CCheat12WeaponCheat1Ev");
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
	f__ZN5CGame7ProcessEv = (int(*)())dlsym(g_dllGTASA, "_ZN5CGame7ProcessEv");
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

void printhelp()
{
	fprintf(stderr, 
		"####################################################\n"
		"				Usage: input cmd\n"
		"help		(print commands)\n"
		"exit		(server cannot exit normally if not call 'exit' commonds when exit exe)\n"
		"1		(WEAPON1)\n"
		"2		(WEAPON2)\n"
		"3		(WEAPON3)\n"
		"4		(WEAPON4)\n"
		"5		(TIMETRAVEL)\n"
		"6		(SCRIPTBYPASS??)\n"
		"7		(SHOWMAPPINGS)\n"
		"8		(INVINCIBILITY)\n"
		"9		(SHOWTAPTOTARGET)\n"
		"10		(SHOWTARGETING)\n"
		"11		(MONEYARMOURHEALTH)\n"
		"12		(WANTEDLEVELUP)\n"
		"13		(WANTEDLEVELDOWN)\n"
		"14		(SUNNYWEATHER)\n"
		"15		(CLOUDYWEATHER)\n"
		"16		(RAINYWEATHER)\n"
		"17		(FOGGYWEATHER)\n"
		"18		(FASTTIME)\n"
		"19		(SLOWTIME)\n"
		"20		(MAYHEM)\n"
		"21		(EVERYBODYATTACKSPLAYER)\n"
		"22		(TANK)\n"
		"23		(STOCKCAR)\n"
		"24		(STOCKCAR2)\n"
		"25		(STOCKCAR3)\n"
		"26		(STOCKCAR4)\n"
		"27		(HEARSE)\n"
		"28		(LOVEFIST)\n"
		"29		(TRASHMASTER)\n"
		"30		(GOLFCART)\n"
		"31		(BLOWUPCARS)\n"
		"32		(SUICIDE)\n"
		"33		(PINKCARS)\n"
		"34		(BLACKCARS)\n"
		"35		(FAT)\n"
		"36		(MUSCLE)\n"
		"37		(THEGAMBLER)\n"
		"38		(SKINNY)\n"
		"39		(ELVISLIVES)\n"
		"40		(VILLAGEPEOPLE)\n"
		"41		(BEACHPARTY)\n"
		"42		(GANGS)\n"
		"43		(GANGLAND)\n"
		"44		(NINJA)\n"
		"45		(LOVECONQUERSALL)\n"
		"46		(ALLCARSARESHIT)\n"
		"47		(ALLCARSAREGREAT)\n"
		"48		(FLYBOY)\n"
		"49		(VORTEX)\n"
		"50		(MIDNIGHT)\n"
		"51		(DUSK)\n"
		"52		(STORM)\n"
		"53		(SANDSTORM)\n"
		"54		(PREDATOR)\n"
		"55		(PARACHUTE)\n"
		"56		(JETPACK)\n"
		"57		(NOTWANTED)\n"
		"58		(WANTED)\n"
		"59		(RIOT)\n"
		"60		(FUNHOUSE)\n"
		"61		(ADRENALINE)\n"
		"62		(DRIVEBY)\n"
		"63		(COUNTRYSIDEINVASION)\n"
		"64		(STAMINA)\n"
		"65		(WEAPONSKILLS)\n"
		"66		(VEHICLESKILLS)\n"
		"67		(APACHE)\n"
		"68		(QUAD)\n"
		"69		(TANKER)\n"
		"70		(DOZER)\n"
		"71		(STUNTPLANE)\n"
		"72		(MONSTERTRUCK)\n"
		"73		(XBOX_HELPER)\n"
		"####################################################\n"
		"\n");
}
map<int,string> callMap;
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
	
	printhelp();
	callMap[1]  = "_ZN6CCheat12WeaponCheat1Ev";
	callMap[2]  = "_ZN6CCheat12WeaponCheat2Ev";
	callMap[3]  = "_ZN6CCheat12WeaponCheat3Ev";
	callMap[4]  = "_ZN6CCheat12WeaponCheat4Ev";
	callMap[5]  = "_ZN6CCheat15TimeTravelCheatEv";
	callMap[6]  = "_ZN6CCheat17ScriptBypassCheatEv";
	callMap[7]	= "_ZN6CCheat17ShowMappingsCheatEv";
	callMap[8]  = "_ZN6CCheat25TogglePlayerInvincibilityEv";
	callMap[9]  = "_ZN6CCheat21ToggleShowTapToTargetEv";
	callMap[10] = "_ZN6CCheat19ToggleShowTargetingEv";
	callMap[11] = "_ZN6CCheat22MoneyArmourHealthCheatEv";
	callMap[12] = "_ZN6CCheat18WantedLevelUpCheatEv";
	callMap[13] = "_ZN6CCheat20WantedLevelDownCheatEv";
	callMap[14] = "_ZN6CCheat22ExtraSunnyWeatherCheatEv";
	callMap[15] = "_ZN6CCheat18CloudyWeatherCheatEv";
	callMap[16] = "_ZN6CCheat17RainyWeatherCheatEv";
	callMap[17] = "_ZN6CCheat17FoggyWeatherCheatEv";
	callMap[18] = "_ZN6CCheat13FastTimeCheatEv";
	callMap[19] = "_ZN6CCheat13SlowTimeCheatEv";
	callMap[20] = "_ZN6CCheat11MayhemCheatEv";
	callMap[21] = "_ZN6CCheat27EverybodyAttacksPlayerCheatEv";
	callMap[22] = "_ZN6CCheat9TankCheatEv";
	callMap[23] = "_ZN6CCheat13StockCarCheatEv";
	callMap[24] = "_ZN6CCheat14StockCar2CheatEv";
	callMap[25] = "_ZN6CCheat14StockCar3CheatEv";
	callMap[26] = "_ZN6CCheat14StockCar4CheatEv";
	callMap[27] = "_ZN6CCheat11HearseCheatEv";
	callMap[28] = "_ZN6CCheat13LovefistCheatEv";
	callMap[29] = "_ZN6CCheat16TrashmasterCheatEv";
	callMap[30] = "_ZN6CCheat13GolfcartCheatEv";
	callMap[31] = "_ZN6CCheat15BlowUpCarsCheatEv";
	callMap[32] = "_ZN6CCheat12SuicideCheatEv";
	callMap[33] = "_ZN6CCheat13PinkCarsCheatEv";
	callMap[34] = "_ZN6CCheat14BlackCarsCheatEv";
	callMap[35] = "_ZN6CCheat8FatCheatEv";
	callMap[36] = "_ZN6CCheat11MuscleCheatEv";
	callMap[37] = "_ZN6CCheat15TheGamblerCheatEv";
	callMap[38] = "_ZN6CCheat11SkinnyCheatEv";
	callMap[39] = "_ZN6CCheat15ElvisLivesCheatEv";
	callMap[40] = "_ZN6CCheat18VillagePeopleCheatEv";
	callMap[41] = "_ZN6CCheat15BeachPartyCheatEv";
	callMap[42] = "_ZN6CCheat10GangsCheatEv";
	callMap[43] = "_ZN6CCheat13GangLandCheatEv";
	callMap[44] = "_ZN6CCheat10NinjaCheatEv";
	callMap[45] = "_ZN6CCheat20LoveConquersAllCheatEv";
	callMap[46] = "_ZN6CCheat19AllCarsAreShitCheatEv";
	callMap[47] = "_ZN6CCheat20AllCarsAreGreatCheatEv";
	callMap[48] = "_ZN6CCheat11FlyboyCheatEv";
	callMap[49] = "_ZN6CCheat11VortexCheatEv";
	callMap[50] = "_ZN6CCheat13MidnightCheatEv";
	callMap[51] = "_ZN6CCheat9DuskCheatEv";
	callMap[52] = "_ZN6CCheat10StormCheatEv";
	callMap[53] = "_ZN6CCheat14SandstormCheatEv";
	callMap[54] = "_ZN6CCheat13PredatorCheatEv";
	callMap[55] = "_ZN6CCheat14ParachuteCheatEv";
	callMap[56] = "_ZN6CCheat12JetpackCheatEv";
	callMap[57] = "_ZN6CCheat14NotWantedCheatEv";
	callMap[58] = "_ZN6CCheat11WantedCheatEv";
	callMap[59] = "_ZN6CCheat9RiotCheatEv";
	callMap[60] = "_ZN6CCheat13FunhouseCheatEv";
	callMap[61] = "_ZN6CCheat15AdrenalineCheatEv";
	callMap[62] = "_ZN6CCheat12DrivebyCheatEv";
	callMap[63] = "_ZN6CCheat24CountrysideInvasionCheatEv";
	callMap[64] = "_ZN6CCheat12StaminaCheatEv";
	callMap[65] = "_ZN6CCheat17WeaponSkillsCheatEv";
	callMap[66] = "_ZN6CCheat18VehicleSkillsCheatEv";
	callMap[67] = "_ZN6CCheat11ApacheCheatEv";
	callMap[68] = "_ZN6CCheat9QuadCheatEv";
	callMap[69] = "_ZN6CCheat11TankerCheatEv";
	callMap[70] = "_ZN6CCheat10DozerCheatEv";
	callMap[71] = "_ZN6CCheat15StuntPlaneCheatEv";
	callMap[72] = "_ZN6CCheat17MonsterTruckCheatEv";
	callMap[73] = "_ZN6CCheat15xboxHelperCheatEv";
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
			printhelp();
		}
		else
		{
			int cheatIndex = 0;
			sscanf(cmdBuf, "%d", &cheatIndex);
			LOGE("cheat index %d\n", cheatIndex);
			char cmd[80] = { 0 };
			int io_length = snprintf(cmd, sizeof(cmd), "D: %s\n", (callMap[cheatIndex]).c_str());
			send(g_socket, cmd, io_length, 0);
		}
	}
	
	
	fprintf(stderr, "socket close %d\n", g_socket);
	close(g_socket);
	return 0;
}