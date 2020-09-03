#include "cheatSDK.h"

CCheatMgr::~CCheatMgr()
{
	if (_pCheat)
	{
		delete _pCheat;
		_pCheat = nullptr;
	}
};

CCheatI* CCheatMgr::createCheatPtr(GAME_NAME game)
{
	switch (game)
	{
	case _GAME_GTASA:
		_pCheat = new CCheatGTASA();
		break;
	case _GAME_GTAVC:
		_pCheat = new CCheatGTAVC();
		break;
	}
	return _pCheat;
};

