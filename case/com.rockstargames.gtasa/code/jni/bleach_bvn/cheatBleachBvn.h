#ifndef _CHEAT_BLENCH_BVN_H
#define _CHEAT_BLENCH_BVN_H

#include <string>
#include <map>
#include <mutex>
using namespace std;

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
	
	string& getFunSymByIndex(int index) {return _callMap[index];}; 
	virtual void setCallFunFlag(const string& funSym, bool bCall = true){};
	virtual bool getFunCalllFlag(const string& funSym){};
	
	virtual void printHelp() = 0;
	virtual void initCallMap() = 0;
	virtual void initCheat() = 0;
	virtual void callFunByFlag() = 0;

protected:	
	map<int, string> _callMap;
	map<string, CheatUnit> _cheatData;
};

class CCheatBlenchBvn : public CCheatI
{
public:
	CCheatBlenchBvn() {};
	virtual ~CCheatBlenchBvn(){};
	
	virtual void setCallFunFlag(const string& funSym, bool bCall = true);
	virtual bool getFunCalllFlag(const string& funSym);
	
	virtual void printHelp();
	virtual void initCallMap();
	virtual void initCheat();
	virtual void callFunByFlag(){};
	
private:
	mutex _cheatMutex;
};

#endif