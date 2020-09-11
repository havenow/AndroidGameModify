#include "cheatI.h"
#include "common.h"

void CCheatGTAVC::printHelp()
{
	fprintf(stderr, 
		"####################################################\n"
		"				Usage: input cmd\n"
		"help		(print commands)\n"
		"exit		(server cannot exit normally if not call 'exit' commonds when exit exe)\n"
		"1		_Z12WeaponCheat1v\n"
		"2		_Z16FannyMagnetCheatv\n"
		"3		_Z12WeaponCheat2v\n"
		"4		_Z12WeaponCheat3v\n"
		"5		_Z11HealthCheath\n"
		"6		_Z15FlyingFishCheatv\n"
		"7		_Z12VehicleCheati	(int a1)\n"
		"8		_Z17PickUpChicksCheatv\n"
		"9		Z16SpecialCarCheatsv\n"
		"10		_Z15BackToTheFuturev\n"
		"11		_Z14BlackCarsCheatv\n"
		"12		_Z13PinkCarsCheatv\n"
		"13		_Z12MadCarsCheatv\n"
		"14		_Z10FunnyCheatv\n"
		"15		_Z13StockCarCheatv\n"
		"16		_Z11HearseCheatv\n"
		"17		_Z13LovefistCheatv\n"
		"18		_Z16TrashmasterCheatv\n"
		"19		_Z15SabreTurboCheatv\n"
		"20		_Z13GolfcartCheatv\n"
		"21		_Z14StockCar2Cheatv\n"
		"22		_Z14StockCar3Cheatv\n"
		"23		_Z14StockCar4Cheatv\n"
		"24		_Z17ChangePlayerCheatPKc	(const char *a1)\n"
		"25		_Z15BlowUpCarsCheatv\n"
		"26		_Z17ChangePlayerCheatv\n"
		"27		_Z11MayhemCheatv\n"
		"28		_Z27EverybodyAttacksPlayerCheatv\n"
		"29		_Z18WeaponsForAllCheatv\n"
		"30		_Z18TrafficLightsCheatv\n"
		"31		_Z13FastTimeCheatv\n"
		"32		_Z13SlowTimeCheatv\n"
		"33		_Z11ArmourCheatv\n"
		"34		_Z18WantedLevelUpCheatv\n"
		"35		_Z12SuicideCheatv\n"
		"36		_Z20WantedLevelDownCheatv\n"
		"37		_Z17SunnyWeatherCheatv\n"
		"38		_Z22ExtraSunnyWeatherCheatv\n"
		"39		_Z18CloudyWeatherCheatv\n"
		"40		_Z17RainyWeatherCheatv\n"
		"41		_Z17FoggyWeatherCheatv\n"
		"42		_Z16FastWeatherCheatv\n"
		"43		_Z21OnlyRenderWheelsCheatv\n"
		"44		_Z21DoChicksWithGunsCheatv\n"
		"45		_Z25ChittyChittyBangBangCheatv\n"
		"46		_Z15StrongGripCheatv\n"
		"47		_Z20DoShowChaseStatCheatv\n"
		"####################################################\n"
		"\n");
}

void CCheatGTAVC::initCallMap()
{
	_callMap[1]  = "_Z12WeaponCheat1v";
	_callMap[2]  = "_Z16FannyMagnetCheatv";
	_callMap[3]  = "_Z12WeaponCheat2v";
	_callMap[4]  = "_Z12WeaponCheat3v";
	_callMap[5]  = "_Z11HealthCheath";
	_callMap[6]  = "_Z15FlyingFishCheatv";
	_callMap[7]  = "_Z12VehicleCheati";//int __fastcall VehicleCheat(int a1) 无参数会 crash
	_callMap[8]  = "_Z17PickUpChicksCheatv";
	_callMap[9]  = "_Z16SpecialCarCheatsv";
	_callMap[10]  = "_Z15BackToTheFuturev";
	_callMap[11]  = "_Z14BlackCarsCheatv";
	_callMap[12]  = "_Z13PinkCarsCheatv";
	_callMap[13]  = "_Z12MadCarsCheatv";
	_callMap[14]  = "_Z10FunnyCheatv";
	_callMap[15]  = "_Z13StockCarCheatv";
	_callMap[16]  = "_Z11HearseCheatv";
	_callMap[17]  = "_Z13LovefistCheatv";
	_callMap[18]  = "_Z16TrashmasterCheatv";
	_callMap[19]  = "_Z15SabreTurboCheatv";
	_callMap[20]  = "_Z13GolfcartCheatv";
	_callMap[21]  = "_Z14StockCar2Cheatv";
	_callMap[22]  = "_Z14StockCar3Cheatv";
	_callMap[23]  = "_Z14StockCar4Cheatv";
	_callMap[24]  = "_Z17ChangePlayerCheatPKc";//int __fastcall ChangePlayerCheat(const char *a1) 无参数会crash
	_callMap[25]  = "_Z15BlowUpCarsCheatv";
	_callMap[26]  = "_Z17ChangePlayerCheatv";//int ChangePlayerCheat(void)
	_callMap[27]  = "_Z11MayhemCheatv";
	_callMap[28]  = "_Z27EverybodyAttacksPlayerCheatv";
	_callMap[29]  = "_Z18WeaponsForAllCheatv";
	_callMap[30]  = "_Z18TrafficLightsCheatv";
	_callMap[31]  = "_Z13FastTimeCheatv";
	_callMap[32]  = "_Z13SlowTimeCheatv";
	_callMap[33]  = "_Z11ArmourCheatv";
	_callMap[34]  = "_Z18WantedLevelUpCheatv";
	_callMap[35]  = "_Z12SuicideCheatv";
	_callMap[36]  = "_Z20WantedLevelDownCheatv";
	_callMap[37]  = "_Z17SunnyWeatherCheatv";
	_callMap[38]  = "_Z22ExtraSunnyWeatherCheatv";
	_callMap[39]  = "_Z18CloudyWeatherCheatv";
	_callMap[40]  = "_Z17RainyWeatherCheatv";
	_callMap[41]  = "_Z17FoggyWeatherCheatv";
	_callMap[42]  = "_Z16FastWeatherCheatv";
	_callMap[43]  = "_Z21OnlyRenderWheelsCheatv";
	_callMap[44]  = "_Z21DoChicksWithGunsCheatv";
	_callMap[45]  = "_Z25ChittyChittyBangBangCheatv";
	_callMap[46]  = "_Z15StrongGripCheatv";
	_callMap[47]  = "_Z20DoShowChaseStatCheatv";
}

void CCheatGTAVC::initCheat(void* dll)
{
	map<int, string>::iterator item;
	for (item = _callMap.begin(); item != _callMap.end(); item++)
	{
		string cheat = item->second;
		
		CheatUnit obj;
		strcpy(obj.strCheatName, cheat.c_str());
		if (dll)
		{
			if (strcmp(obj.strCheatName, "_Z12VehicleCheati") == 0)
			{
				obj.pfunCheat2 = (int(*)(int))dlsym(dll, obj.strCheatName);
			}
			else if (strcmp(obj.strCheatName, "_Z17ChangePlayerCheatPKc") == 0)
			{
				obj.pfunCheat3 = (int(*)(const char*))dlsym(dll, obj.strCheatName);
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

void CCheatGTAVC::callFunByFlag()
{
	std::map<string, CheatUnit>::iterator item;
	for (item = _cheatData.begin(); item != _cheatData.end(); item++)
	{
		if (item->second.bCall)
		{
			LOGE("replaceGameProcess %s %d\n", item->second.strCheatName, item->second.bCall);
			if (strcmp(item->second.strCheatName, "_Z12VehicleCheati") == 0)
			{
				//int __fastcall VehicleCheat(int a1)
				//(item->second.pfunCheat2)(1);
			}
			else if(strcmp(item->second.strCheatName, "_Z17ChangePlayerCheatPKc") == 0)
			{
				//int __fastcall ChangePlayerCheat(const char *a1)
				//(item->second.pfunCheat3)("play2");
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