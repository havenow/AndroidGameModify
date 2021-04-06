#include "cheatI.h"
#include "common.h"

void CCheatBULLY::printHelp()
{
	fprintf(stderr, 
		"####################################################\n"
		"				Usage: input cmd\n"
		"help		(print commands)\n"
		"exit		(server cannot exit normally if not call 'exit' commonds when exit exe)\n"
		"sdkFinish	(clear cheat sdk)\n"
		"1		(IncreaseHealth active)\n"
		"2		(OneShotKill toggle)\n"
		"3		(PunishmentPointsDecrease)\n"
		"4		(IncreaseCashActivate)\n"
		"5		(AllWeaponsActivate)\n"
		"6		(MaxAmmoActivate)\n"
		"7		(SpawnVehicle)\n"
		"8		(Invulnerability Toggle)\n"
		"9		(PassMission Activate)\n"
		"10		(FailMission Activate)\n"
		"11		(PunishmentPoints Increase)\n"
		"12		(SpawnPed Spawn)\n"
		"13		(TeleportToArea Teleport)\n"
		"14		(ChangeSeason NextSeason)\n"
		"15		(ChangeSeason NextWeather)\n"
		"16		(TurnOffPlayerAttacks Toggle)\n"
		"####################################################\n"
		"\n");
}

void CCheatBULLY::initCallMap()
{
	_callMap[1]  = "_ZN19CheatIncreaseHealth8ActivateEv";
	_callMap[2]  = "_ZN17CheatOneShotKills6ToggleEv";
	_callMap[3]  = "_ZN21CheatPunishmentPoints8DecreaseEv";
	_callMap[4]  = "_ZN17CheatIncreaseCash8ActivateEv";
	_callMap[5]  = "_ZN15CheatAllWeapons8ActivateEv";
	_callMap[6]  = "_ZN12CheatMaxAmmo8ActivateEv";
	_callMap[7]	 = "_ZN17CheatSpawnVehicle5SpawnEv";
	
	_callMap[8]	 = "_ZN20CheatInvulnerability6ToggleEv";
	_callMap[9]	 = "_ZN16CheatPassMission8ActivateEv";
	_callMap[10]	 = "_ZN16CheatFailMission8ActivateEv";
	_callMap[11]	 = "_ZN21CheatPunishmentPoints8IncreaseEv";
	_callMap[12]	 = "_ZN13CheatSpawnPed5SpawnEv";
	_callMap[13]	 = "_ZN19CheatTeleportToArea8TeleportEv";
	_callMap[14]	 = "_ZN17CheatChangeSeason10NextSeasonEv";
	_callMap[15]	 = "_ZN17CheatChangeSeason11NextWeatherEv";
	_callMap[16]	 = "_ZN25CheatTurnOffPlayerAttacks6ToggleEv";
}

void CCheatBULLY::initCheat(void* dll)
{
	map<int, string>::iterator item;
	for (item = _callMap.begin(); item != _callMap.end(); item++)
	{
		string cheat = item->second;
		
		CheatUnit obj;
		strcpy(obj.strCheatName, cheat.c_str());
		if (dll)
		{
			obj.pfunCheat = (int(*)())dlsym(dll, obj.strCheatName);
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
		_cheatData[cheat] = obj;
	}
}

void CCheatBULLY::callFunByFlag()
{
	std::map<string, CheatUnit>::iterator item;
	for (item = _cheatData.begin(); item != _cheatData.end(); item++)
	{
		if (item->second.bCall)
		{
			LOGE("replaceGameProcess %s %d\n", item->second.strCheatName, item->second.bCall);
			
			if (item->second.pfunCheat)
				(item->second.pfunCheat)();
			else
				LOGE("item->second.pfunCheat is null\n");
			
			item->second.bCall = false;
		}
	}
}
