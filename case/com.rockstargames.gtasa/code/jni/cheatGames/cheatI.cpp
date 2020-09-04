#include "cheatI.h"
#include "common.h"

void CCheatI::setCallFunFlag(const string& funSym)
{
	auto pItem = _cheatData.find(funSym);
	if (pItem != _cheatData.end())
	{
		(*pItem).second.bCall = true;
		LOGE("process_cmd: %s", (*pItem).second.strCheatName);
	}
	else
		LOGE("cant find process_cmd: %s", funSym.c_str()); 
}

string& CCheatI::getFunSymByIndex(int index)
{
	return _callMap[index];
}