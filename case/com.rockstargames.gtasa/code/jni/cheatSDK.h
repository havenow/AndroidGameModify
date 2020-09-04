#ifndef _CHEAT_SDK_H
#define _CHEAT_SDK_H

#include <string>
#include <map>
#include <mutex>

#include "cheatGames\cheatI.h"
#include "hookGames\hookStrategy.h"

using namespace std;

class CCheatSDK
{
public:
	static CCheatSDK* getInstance()
	{
		static CCheatSDK obj;
		return &obj;
	}
	void loadSo(GAME_NAME game);
	CCheatI* chooseCheatGame(GAME_NAME game);
	
	string& getFunSymByIndex(int index) {if (_pCheat) return _pCheat->getFunSymByIndex(index);}
	void setCallFunFlag(const string& funSym);
	
	void printHelp() {if (_pCheat) _pCheat->printHelp();}
	void initCallMap() {if (_pCheat) _pCheat->initCallMap();}
	void initCheat() {if (_pCheat) _pCheat->initCheat(_dll);}
	void callFunByFlag();
	
	void chooseHookStrategy(GAME_NAME game);
	void setStrategyDll()	{ _hookInstance.setDll(_dll); }
	void DoHook(){ _hookInstance.DoHook(); }

protected:
	CCheatSDK() {};
	~CCheatSDK();
	CCheatSDK(const CCheatSDK&) = delete;
	CCheatSDK& operator= (const CCheatSDK&) = delete;
	
private:
	CCheatI* _pCheat = nullptr;
	hookStrategy* _hookStrategy = nullptr;
	hookGame _hookInstance;
	mutex _cheatMutex;
	void* _dll = nullptr;
};

#endif