#include "cheatBleachBvn.h"
#include "common.h"

void CCheatBlenchBvn::printHelp()
{
	fprintf(stderr, 
		"####################################################\n"
		"				Usage: input cmd\n"
		"help		(print commands)\n"
		"exit		(server cannot exit normally if not call 'exit' commonds when exit exe)\n"
		"sdkFinish	(clear cheat sdk)\n"
		"1		keep_blood\n"
		"2		keep_sp\n"
		"3		keep_blue\n"
		"4		keep_assist\n"
		"####################################################\n"
		"\n");
}

void CCheatBlenchBvn::initCallMap()
{
	_callMap[1]  = "keep_blood";
	_callMap[2]  = "keep_sp";
	_callMap[3]  = "keep_blue";
	_callMap[4]  = "keep_assist";
}

void CCheatBlenchBvn::initCheat()
{
	map<int, string>::iterator item;
	for (item = _callMap.begin(); item != _callMap.end(); item++)
	{
		string cheat = item->second;
		
		CheatUnit obj;
		strcpy(obj.strCheatName, cheat.c_str());
		obj.pfunCheat = nullptr;
		obj.pfunCheat2 = nullptr;
		obj.pfunCheat3 = nullptr;

		obj.bCall = false;
		_cheatData[cheat] = obj;
	}
}

void CCheatBlenchBvn::setCallFunFlag(const string& funSym, bool bCall)
{
	lock_guard<mutex> lock(_cheatMutex);
	auto pItem = _cheatData.find(funSym);
	if (pItem != _cheatData.end())
	{
		(*pItem).second.bCall = bCall;
		LOGE("process_cmd: %s", (*pItem).second.strCheatName);
	}
	else
		LOGE("cant find process_cmd: %s", funSym.c_str()); 
}

bool CCheatBlenchBvn::getFunCalllFlag(const string& funSym)
{
	lock_guard<mutex> lock(_cheatMutex);
	bool ret = false;
	auto pItem = _cheatData.find(funSym);
	if (pItem != _cheatData.end())
	{
		ret = (*pItem).second.bCall;
	}
	return ret;
}
