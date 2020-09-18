#include "cheatI.h"
#include "common.h"

void CCheatGTA3::printHelp()
{
	fprintf(stderr, 
		"####################################################\n"
		"				Usage: input cmd\n"
		"help		(print commands)\n"
		"exit		(server cannot exit normally if not call 'exit' commonds when exit exe)\n"
		"sdkFinish	(clear cheat sdk)\n"
		"1		_Z11WeaponCheatv\n"//bad
		"2		_Z11HealthCheatv\n"
		"3		_Z9TankCheatv\n"
		"4		_Z15BlowUpCarsCheatv\n"
		"5		_Z17ChangePlayerCheatv\n"//bad
		"6		_Z11MayhemCheatv\n"
		"7		_Z27EverybodyAttacksPlayerCheatv\n"
		"8		_Z18WeaponsForAllCheatv\n"
		"9		_Z13FastTimeCheatv\n"
		"10		_Z13SlowTimeCheatv\n"
		"11		_Z10MoneyCheatv\n"
		"12		_Z11ArmourCheatv\n"
		"13		_Z18WantedLevelUpCheatv\n"
		"14		_Z20WantedLevelDownCheatv\n"
		"15		_Z17SunnyWeatherCheatv\n"
		"16		_Z18CloudyWeatherCheatv\n"
		"17		_Z17RainyWeatherCheatv\n"
		"18		_Z17FoggyWeatherCheatv\n"
		"19		_Z16FastWeatherCheatv\n"
		"20		_Z21OnlyRenderWheelsCheatv\n"
		"21		_Z25ChittyChittyBangBangCheatv\n"
		"22		_Z15StrongGripCheatv\n"
		"23		_Z15NastyLimbsCheatv\n"//bad
		//"24		_Z11ClearCheatsv\n"
		"####################################################\n"
		"\n");
}

void CCheatGTA3::initCallMap()
{
	_callMap[1]  = "_Z11WeaponCheatv";
	_callMap[2]  = "_Z11HealthCheatv";
	_callMap[3]  = "_Z9TankCheatv";
	_callMap[4]  = "_Z15BlowUpCarsCheatv";
	_callMap[5]  = "_Z17ChangePlayerCheatv";
	_callMap[6]  = "_Z11MayhemCheatv";
	_callMap[7]  = "_Z27EverybodyAttacksPlayerCheatv";
	_callMap[8]  = "_Z18WeaponsForAllCheatv";
	_callMap[9]  = "_Z13FastTimeCheatv";
	_callMap[10]  = "_Z13SlowTimeCheatv";
	_callMap[11]  = "_Z10MoneyCheatv";
	_callMap[12]  = "_Z11ArmourCheatv";
	_callMap[13]  = "_Z18WantedLevelUpCheatv";
	_callMap[14]  = "_Z20WantedLevelDownCheatv";
	_callMap[15]  = "_Z17SunnyWeatherCheatv";
	_callMap[16]  = "_Z18CloudyWeatherCheatv";
	_callMap[17]  = "_Z17RainyWeatherCheatv";
	_callMap[18]  = "_Z17FoggyWeatherCheatv";
	_callMap[19]  = "_Z16FastWeatherCheatv";
	_callMap[20]  = "_Z21OnlyRenderWheelsCheatv";
	_callMap[21]  = "_Z25ChittyChittyBangBangCheatv";
	_callMap[22]  = "_Z15StrongGripCheatv";
	_callMap[23]  = "_Z15NastyLimbsCheatv";
	//_callMap[24]  = "_Z11ClearCheatsv";
}

void CCheatGTA3::initCheat(void* dll)
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

void CCheatGTA3::callFunByFlag()
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