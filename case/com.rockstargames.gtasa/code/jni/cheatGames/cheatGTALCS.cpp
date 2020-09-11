#include "cheatI.h"
#include "common.h"

void CCheatGTALCS::printHelp()
{
	fprintf(stderr, 
		"####################################################\n"
		"				Usage: input cmd\n"
		"help		(print commands)\n"
		"exit		(server cannot exit normally if not call 'exit' commonds when exit exe)\n"
		"1		_Z12WeaponCheat1v\n"
		"2		_Z12WeaponCheat2v\n"
		"3		_Z12WeaponCheat3v\n"
		"4		_Z15BikeWheelsCheatv\n"
		"5		_Z17WallClimbingCheatv\n"
		"6		_Z14GlassCarsCheatv\n"
		"7		_Z11HealthCheath	(int a1)\n"
		"8		_Z9TankCheatv\n"
		"9		_Z16TrashmasterCheatv\n"
		"10		_Z15BlowUpCarsCheatv\n"
		"11		_Z17ChangePlayerCheatv\n"
		"12		_Z11MayhemCheatv\n"
		"13		_Z27EverybodyAttacksPlayerCheatv\n"
		"14		_Z18WeaponsForAllCheatv\n"
		"15		_Z13FastTimeCheatv\n"
		"16		_Z13SlowTimeCheatv\n"
		"17		_Z16FannyMagnetCheatv\n"
		"18		_Z10MoneyCheatv\n"
		"19		_Z11ArmourCheatv\n"
		"20		_Z18WantedLevelUpCheatv\n"
		"21		_Z20WantedLevelDownCheatv\n"
		"22		_Z17SunnyWeatherCheatv\n"
		"23		_Z22ExtraSunnyWeatherCheatv\n"
		"24		_Z18CloudyWeatherCheatv\n"
		"25		_Z17RainyWeatherCheatv\n"
		"26		_Z17FoggyWeatherCheatv\n"
		"27		_Z16FastWeatherCheatv\n"
		"28		_Z14SlowClockCheatv\n"
		"29		_Z21OnlyRenderWheelsCheatv\n"
		"30		_Z15StrongGripCheatv\n"
		"31		_Z13BigHeadsCheatv\n"
		"32		_Z23MultiplayerUnlockCheat1v\n"
		"33		_Z23MultiplayerUnlockCheat2v\n"
		"34		_Z23MultiplayerUnlockCheat3v\n"
		"35		_Z23MultiplayerUnlockCheat4v\n"
		"36		_Z20DoShowChaseStatCheatv\n"
		"37		_Z21DoChicksWithGunsCheatv\n"
		"38		_Z12SuicideCheatv\n"
		"39		_Z18TrafficLightsCheatv\n"
		"40		_Z12MadCarsCheatv\n"
		"41		_Z13PinkCarsCheatv\n"
		"42		_Z14BlackCarsCheatv\n"
		"43		_Z17PickUpChicksCheatv\n"
		"44		_Z15FlyingFishCheatv\n"
		"####################################################\n"
		"\n");
}

void CCheatGTALCS::initCallMap()
{
	_callMap[1]  = "_Z12WeaponCheat1v";
	_callMap[2]  = "_Z12WeaponCheat2v";
	_callMap[3]  = "_Z12WeaponCheat3v";
	_callMap[4]  = "_Z15BikeWheelsCheatv";
	_callMap[5]  = "_Z17WallClimbingCheatv";
	_callMap[6]  = "_Z14GlassCarsCheatv";
	_callMap[7]  = "_Z11HealthCheath";//int __fastcall HealthCheat(int a1)
	_callMap[8]  = "_Z9TankCheatv";
	_callMap[9]  = "_Z16TrashmasterCheatv";
	_callMap[10]  = "_Z15BlowUpCarsCheatv";
	_callMap[11]  = "_Z17ChangePlayerCheatv";
	_callMap[12]  = "_Z11MayhemCheatv";
	_callMap[13]  = "_Z27EverybodyAttacksPlayerCheatv";
	_callMap[14]  = "_Z18WeaponsForAllCheatv";
	_callMap[15]  = "_Z13FastTimeCheatv";
	_callMap[16]  = "_Z13SlowTimeCheatv";
	_callMap[17]  = "_Z16FannyMagnetCheatv";
	_callMap[18]  = "_Z10MoneyCheatv";
	_callMap[19]  = "_Z11ArmourCheatv";
	_callMap[20]  = "_Z18WantedLevelUpCheatv";
	_callMap[21]  = "_Z20WantedLevelDownCheatv";
	_callMap[22]  = "_Z17SunnyWeatherCheatv";
	_callMap[23]  = "_Z22ExtraSunnyWeatherCheatv";
	_callMap[24]  = "_Z18CloudyWeatherCheatv";
	_callMap[25]  = "_Z17RainyWeatherCheatv";
	_callMap[26]  = "_Z17FoggyWeatherCheatv";
	_callMap[27]  = "_Z16FastWeatherCheatv";
	_callMap[28]  = "_Z14SlowClockCheatv";
	_callMap[29]  = "_Z21OnlyRenderWheelsCheatv";
	_callMap[30]  = "_Z15StrongGripCheatv";
	_callMap[31]  = "_Z13BigHeadsCheatv";
	_callMap[32]  = "_Z23MultiplayerUnlockCheat1v";
	_callMap[33]  = "_Z23MultiplayerUnlockCheat2v";
	_callMap[34]  = "_Z23MultiplayerUnlockCheat3v";
	_callMap[35]  = "_Z23MultiplayerUnlockCheat4v";
	_callMap[36]  = "_Z20DoShowChaseStatCheatv";
	_callMap[37]  = "_Z21DoChicksWithGunsCheatv";
	_callMap[38]  = "_Z12SuicideCheatv";
	_callMap[39]  = "_Z18TrafficLightsCheatv";
	_callMap[40]  = "_Z12MadCarsCheatv";
	_callMap[41]  = "_Z13PinkCarsCheatv";
	_callMap[42]  = "_Z14BlackCarsCheatv";
	_callMap[43]  = "_Z17PickUpChicksCheatv";
	_callMap[44]  = "_Z15FlyingFishCheatv";
}

void CCheatGTALCS::initCheat(void* dll)
{
	map<int, string>::iterator item;
	for (item = _callMap.begin(); item != _callMap.end(); item++)
	{
		string cheat = item->second;
		
		CheatUnit obj;
		strcpy(obj.strCheatName, cheat.c_str());
		if (dll)
		{
			if (strcmp(obj.strCheatName, "_Z11HealthCheath") == 0)
			{
				obj.pfunCheat2 = (int(*)(int))dlsym(dll, obj.strCheatName);
			}
			else
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

void CCheatGTALCS::callFunByFlag()
{
	std::map<string, CheatUnit>::iterator item;
	for (item = _cheatData.begin(); item != _cheatData.end(); item++)
	{
		if (item->second.bCall)
		{
			LOGE("replaceGameProcess %s %d\n", item->second.strCheatName, item->second.bCall);
			if (strcmp(item->second.strCheatName, "_Z11HealthCheath") == 0)//1 0 ???
			{
				//int __fastcall HealthCheat(int a1)
				//(item->second.pfunCheat2)(1);
			}
			else
			{
				if (item->second.pfunCheat)
					(item->second.pfunCheat)();
				else
					LOGE("item->second.pfunCheat is null\n");
			}
			item->second.bCall = false;
		}
	}
}