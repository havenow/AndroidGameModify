#include "cheatI.h"
#include "common.h"

void CCheatDadNMe::printHelp()
{
	fprintf(stderr, 
		"####################################################\n"
		"				Usage: input cmd\n"
		"help		(print commands)\n"
		"exit		(server cannot exit normally if not call 'exit' commonds when exit exe)\n"
		"sdkFinish	(clear cheat sdk)\n"
		"1		keep_blood\n"
		"2		keep_crazy\n"
		"####################################################\n"
		"\n");
}

void CCheatDadNMe::initCallMap()
{
	_callMap[1]  = "keep_blood";
	_callMap[2]  = "keep_crazy";
}

void CCheatDadNMe::initCheat(void* dll)
{
	map<int, string>::iterator item;
	for (item = _callMap.begin(); item != _callMap.end(); item++)
	{
		string cheat = item->second;
		
		CheatUnit obj;
		strcpy(obj.strCheatName, cheat.c_str());
		obj.pfunCheat = nullptr;

		obj.bCall = false;
		_cheatData[cheat] = obj;
	}
}

void CCheatDadNMe::setCallFunFlag(const string& funSym, bool bCall)
{
	auto pItem = _cheatData.find(funSym);
	if (pItem != _cheatData.end())
	{
		(*pItem).second.bCall = bCall;
		LOGE("process_cmd: %s", (*pItem).second.strCheatName);
	}
	else
		LOGE("cant find process_cmd: %s", funSym.c_str()); 
}

bool CCheatDadNMe::getFunCalllFlag(const string& funSym)
{
	bool ret = false;
	auto pItem = _cheatData.find(funSym);
	if (pItem != _cheatData.end())
	{
		ret = (*pItem).second.bCall;
	}
	return ret;
}