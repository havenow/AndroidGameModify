#include "cheatSDK.h"
#include "common.h"

CCheatSDK::~CCheatSDK()
{
	if (_pCheat)
	{
		delete _pCheat;
		_pCheat = nullptr;
	}
	if (_hookStrategy)
	{
		delete _hookStrategy;
		_hookStrategy = nullptr;
	}
};

void CCheatSDK::loadSo(GAME_NAME game)
{
	pid_t target_pid = 0; 
	char *error;
	switch (game)
	{
	case _GAME_GTASA:
		target_pid = find_pid_of("com.rockstargames.gtasa");
		appendToLogFile("target_pid %d.\n", target_pid);
		LOGE("target_pid %d.\n", target_pid);
		_dll = dlopen("libGTASA.so", RTLD_NOW);
		break;
	case _GAME_GTAVC:
		target_pid = find_pid_of("com.rockstargames.gtavc");
		appendToLogFile("target_pid %d.\n", target_pid);
		LOGE("target_pid %d.\n", target_pid);
		_dll = dlopen("libGTAVC.so", RTLD_NOW);
		break;
	case _GAME_GTALCS:
		target_pid = find_pid_of("com.rockstargames.gtalcs");
		appendToLogFile("target_pid %d.\n", target_pid);
		LOGE("target_pid %d.\n", target_pid);
		_dll = dlopen("libGTALcs.so", RTLD_NOW);
	case _GAME_GTA3:
		target_pid = find_pid_of("com.rockstar.gta3");
		appendToLogFile("target_pid %d.\n", target_pid);
		LOGE("target_pid %d.\n", target_pid);
		_dll = dlopen("libR1.so", RTLD_NOW);
		break;
	case _GAME_DADNME:
		target_pid = find_pid_of("com.lakegame.dadnme");
		appendToLogFile("target_pid %d.\n", target_pid);
		LOGE("target_pid %d.\n", target_pid);
		_dll = dlopen("libc.so", RTLD_NOW);
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
	
	if(_dll==NULL)
	{
		appendToLogFile("dlsym error dll == NULL\n");
		LOGE("dlsym error dll == NULL\n");
	}
		
	else
	{
		appendToLogFile("dlsym dll != NULL\n");
		LOGE("dlsym dll != NULL\n");
	}
}

CCheatI* CCheatSDK::chooseCheatGame(GAME_NAME game)
{
	switch (game)
	{
	case _GAME_GTASA:
		_pCheat = new CCheatGTASA();
		break;
	case _GAME_GTAVC:
		_pCheat = new CCheatGTAVC();
		break;
	case _GAME_GTALCS:
		_pCheat = new CCheatGTALCS();
		break;
	case _GAME_GTA3:
		_pCheat = new CCheatGTA3();
		break;
	case _GAME_DADNME:
		_pCheat = new CCheatDadNMe();
		break;
	}
	return _pCheat;
};

void CCheatSDK::chooseHookStrategy(GAME_NAME game)
{
	switch (game)
	{
	case _GAME_GTASA:
		_hookStrategy = new hookGTASA();
		break;
	case _GAME_GTAVC:
		_hookStrategy = new hookGTAVC();
		break;
	case _GAME_GTALCS:
		_hookStrategy = new hookGTALCS();
		break;
	case _GAME_GTA3:
		_hookStrategy = new hookGTA3();
		break;
	case _GAME_DADNME:
		_hookStrategy = new hookDadNMe();
		break;
	}
	_hookInstance.setStrategy(_hookStrategy);
}

void CCheatSDK::setCallFunFlag(const string& funSym, bool bOpen) 
{ 
	if (_pCheat) 
	{
		lock_guard<mutex> lock(_cheatMutex);
		_pCheat->setCallFunFlag(funSym, bOpen);
	}
		
}

void CCheatSDK::callFunByFlag() 
{ 
	if (_pCheat)
	{
		lock_guard<mutex> lock(_cheatMutex);
		_pCheat->callFunByFlag();
	}
}

void CCheatSDK::cheatSDKFinish()
{
	if (_pCheat)
	{
		delete _pCheat;
		_pCheat = nullptr;
	}
	if (_hookStrategy)
	{
		delete _hookStrategy;
		_hookStrategy = nullptr;
	}
}