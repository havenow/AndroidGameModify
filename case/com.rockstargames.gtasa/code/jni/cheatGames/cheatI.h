#ifndef _CHEAT_INTERFACE_H
#define _CHEAT_INTERFACE_H

#include <string>
#include <map>
using namespace std;

enum GAME_NAME
{
	_GAME_NONE 	= -1,
	_GAME_GTASA = 0,
	_GAME_GTAVC = 1,
	_GAME_GTALCS = 2,
	_GAME_GTA3 = 3,
	_GAME_DADNME = 4,
};


#ifndef _TAG_CHEAT_UNIT_
#define _TAG_CHEAT_UNIT_
typedef struct tagCheatUnit
{
	char strCheatName[128];
	int (*pfunCheat)();
	int (*pfunCheat2)(int);
	int (*pfunCheat3)(const char*);
	bool bCall;
} CheatUnit;
#endif

class CCheatI
{
public:
	CCheatI(){};
	virtual ~CCheatI(){};
	
	string& getFunSymByIndex(int index); 
	virtual void setCallFunFlag(const string& funSym, bool bCall = true);
	virtual bool getFunCalllFlag(const string& funSym){};
	
	virtual void printHelp() = 0;
	virtual void initCallMap() = 0;
	virtual void initCheat(void* dll) = 0;
	virtual void callFunByFlag() = 0;

protected:	
	map<int, string> _callMap;
	map<string, CheatUnit> _cheatData;
};

class CCheatGTASA : public CCheatI
{
public:
	CCheatGTASA() {};
	virtual ~CCheatGTASA() {};
	
	virtual void printHelp();
	virtual void initCallMap();
	virtual void initCheat(void* dll);
	virtual void callFunByFlag();
};


class CCheatGTAVC : public CCheatI
{
public:
	CCheatGTAVC() {};
	virtual ~CCheatGTAVC() {};

	virtual void printHelp();
	virtual void initCallMap();
	virtual void initCheat(void* dll);
	virtual void callFunByFlag();
};

class CCheatGTALCS : public CCheatI
{
public:
	CCheatGTALCS() {};
	virtual ~CCheatGTALCS(){};
	
	virtual void printHelp();
	virtual void initCallMap();
	virtual void initCheat(void* dll);
	virtual void callFunByFlag();
};

class CCheatGTA3 : public CCheatI
{
public:
	CCheatGTA3() {};
	virtual ~CCheatGTA3(){};
	
	virtual void printHelp();
	virtual void initCallMap();
	virtual void initCheat(void* dll);
	virtual void callFunByFlag();
};

class CCheatDadNMe : public CCheatI
{
public:
	CCheatDadNMe() {};
	virtual ~CCheatDadNMe(){};
	
	virtual void setCallFunFlag(const string& funSym, bool bCall = true);
	virtual bool getFunCalllFlag(const string& funSym);
	
	virtual void printHelp();
	virtual void initCallMap();
	virtual void initCheat(void* dll);
	virtual void callFunByFlag(){};
};

#endif