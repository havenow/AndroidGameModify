#include "cheatSDK.h"
#include "common.h"

void CCheatGTASA::printHelp()
{
	fprintf(stderr, 
		"####################################################\n"
		"				Usage: input cmd\n"
		"help		(print commands)\n"
		"exit		(server cannot exit normally if not call 'exit' commonds when exit exe)\n"
		"1		(WEAPON1)\n"
		"2		(WEAPON2)\n"
		"3		(WEAPON3)\n"
		"4		(WEAPON4)\n"
		"5		(TIMETRAVEL)\n"
		"6		(SCRIPTBYPASS??)\n"
		"7		(SHOWMAPPINGS)\n"
		"8		(INVINCIBILITY)\n"
		"9		(SHOWTAPTOTARGET)\n"
		"10		(SHOWTARGETING)\n"
		"11		(MONEYARMOURHEALTH)\n"
		"12		(WANTEDLEVELUP)\n"
		"13		(WANTEDLEVELDOWN)\n"
		"14		(SUNNYWEATHER)\n"
		"15		(CLOUDYWEATHER)\n"
		"16		(RAINYWEATHER)\n"
		"17		(FOGGYWEATHER)\n"
		"18		(FASTTIME)\n"
		"19		(SLOWTIME)\n"
		"20		(MAYHEM)\n"
		"21		(EVERYBODYATTACKSPLAYER)\n"
		"22		(TANK)\n"
		"23		(STOCKCAR)\n"
		"24		(STOCKCAR2)\n"
		"25		(STOCKCAR3)\n"
		"26		(STOCKCAR4)\n"
		"27		(HEARSE)\n"
		"28		(LOVEFIST)\n"
		"29		(TRASHMASTER)\n"
		"30		(GOLFCART)\n"
		"31		(BLOWUPCARS)\n"
		"32		(SUICIDE)\n"
		"33		(PINKCARS)\n"
		"34		(BLACKCARS)\n"
		"35		(FAT)\n"
		"36		(MUSCLE)\n"
		"37		(THEGAMBLER)\n"
		"38		(SKINNY)\n"
		"39		(ELVISLIVES)\n"
		"40		(VILLAGEPEOPLE)\n"
		"41		(BEACHPARTY)\n"
		"42		(GANGS)\n"
		"43		(GANGLAND)\n"
		"44		(NINJA)\n"
		"45		(LOVECONQUERSALL)\n"
		"46		(ALLCARSARESHIT)\n"
		"47		(ALLCARSAREGREAT)\n"
		"48		(FLYBOY)\n"
		"49		(VORTEX)\n"
		"50		(MIDNIGHT)\n"
		"51		(DUSK)\n"
		"52		(STORM)\n"
		"53		(SANDSTORM)\n"
		"54		(PREDATOR)\n"
		"55		(PARACHUTE)\n"
		"56		(JETPACK)\n"
		"57		(NOTWANTED)\n"
		"58		(WANTED)\n"
		"59		(RIOT)\n"
		"60		(FUNHOUSE)\n"
		"61		(ADRENALINE)\n"
		"62		(DRIVEBY)\n"
		"63		(COUNTRYSIDEINVASION)\n"
		"64		(STAMINA)\n"
		"65		(WEAPONSKILLS)\n"
		"66		(VEHICLESKILLS)\n"
		"67		(APACHE)\n"
		"68		(QUAD)\n"
		"69		(TANKER)\n"
		"70		(DOZER)\n"
		"71		(STUNTPLANE)\n"
		"72		(MONSTERTRUCK)\n"
		"73		(XBOX_HELPER)\n"
		"####################################################\n"
		"\n");
}

void CCheatGTASA::initCallMap()
{
	_callMap[1]  = "_ZN6CCheat12WeaponCheat1Ev";
	_callMap[2]  = "_ZN6CCheat12WeaponCheat2Ev";
	_callMap[3]  = "_ZN6CCheat12WeaponCheat3Ev";
	_callMap[4]  = "_ZN6CCheat12WeaponCheat4Ev";
	_callMap[5]  = "_ZN6CCheat15TimeTravelCheatEv";
	_callMap[6]  = "_ZN6CCheat17ScriptBypassCheatEv";
	_callMap[7]	 = "_ZN6CCheat17ShowMappingsCheatEv";
	_callMap[8]  = "_ZN6CCheat25TogglePlayerInvincibilityEv";
	_callMap[9]  = "_ZN6CCheat21ToggleShowTapToTargetEv";
	_callMap[10] = "_ZN6CCheat19ToggleShowTargetingEv";
	_callMap[11] = "_ZN6CCheat22MoneyArmourHealthCheatEv";
	_callMap[12] = "_ZN6CCheat18WantedLevelUpCheatEv";
	_callMap[13] = "_ZN6CCheat20WantedLevelDownCheatEv";
	_callMap[14] = "_ZN6CCheat22ExtraSunnyWeatherCheatEv";
	_callMap[15] = "_ZN6CCheat18CloudyWeatherCheatEv";
	_callMap[16] = "_ZN6CCheat17RainyWeatherCheatEv";
	_callMap[17] = "_ZN6CCheat17FoggyWeatherCheatEv";
	_callMap[18] = "_ZN6CCheat13FastTimeCheatEv";
	_callMap[19] = "_ZN6CCheat13SlowTimeCheatEv";
	_callMap[20] = "_ZN6CCheat11MayhemCheatEv";
	_callMap[21] = "_ZN6CCheat27EverybodyAttacksPlayerCheatEv";
	_callMap[22] = "_ZN6CCheat9TankCheatEv";
	_callMap[23] = "_ZN6CCheat13StockCarCheatEv";
	_callMap[24] = "_ZN6CCheat14StockCar2CheatEv";
	_callMap[25] = "_ZN6CCheat14StockCar3CheatEv";
	_callMap[26] = "_ZN6CCheat14StockCar4CheatEv";
	_callMap[27] = "_ZN6CCheat11HearseCheatEv";
	_callMap[28] = "_ZN6CCheat13LovefistCheatEv";
	_callMap[29] = "_ZN6CCheat16TrashmasterCheatEv";
	_callMap[30] = "_ZN6CCheat13GolfcartCheatEv";
	_callMap[31] = "_ZN6CCheat15BlowUpCarsCheatEv";
	_callMap[32] = "_ZN6CCheat12SuicideCheatEv";
	_callMap[33] = "_ZN6CCheat13PinkCarsCheatEv";
	_callMap[34] = "_ZN6CCheat14BlackCarsCheatEv";
	_callMap[35] = "_ZN6CCheat8FatCheatEv";
	_callMap[36] = "_ZN6CCheat11MuscleCheatEv";
	_callMap[37] = "_ZN6CCheat15TheGamblerCheatEv";
	_callMap[38] = "_ZN6CCheat11SkinnyCheatEv";
	_callMap[39] = "_ZN6CCheat15ElvisLivesCheatEv";
	_callMap[40] = "_ZN6CCheat18VillagePeopleCheatEv";
	_callMap[41] = "_ZN6CCheat15BeachPartyCheatEv";
	_callMap[42] = "_ZN6CCheat10GangsCheatEv";
	_callMap[43] = "_ZN6CCheat13GangLandCheatEv";
	_callMap[44] = "_ZN6CCheat10NinjaCheatEv";
	_callMap[45] = "_ZN6CCheat20LoveConquersAllCheatEv";
	_callMap[46] = "_ZN6CCheat19AllCarsAreShitCheatEv";
	_callMap[47] = "_ZN6CCheat20AllCarsAreGreatCheatEv";
	_callMap[48] = "_ZN6CCheat11FlyboyCheatEv";
	_callMap[49] = "_ZN6CCheat11VortexCheatEv";
	_callMap[50] = "_ZN6CCheat13MidnightCheatEv";
	_callMap[51] = "_ZN6CCheat9DuskCheatEv";
	_callMap[52] = "_ZN6CCheat10StormCheatEv";
	_callMap[53] = "_ZN6CCheat14SandstormCheatEv";
	_callMap[54] = "_ZN6CCheat13PredatorCheatEv";
	_callMap[55] = "_ZN6CCheat14ParachuteCheatEv";
	_callMap[56] = "_ZN6CCheat12JetpackCheatEv";
	_callMap[57] = "_ZN6CCheat14NotWantedCheatEv";
	_callMap[58] = "_ZN6CCheat11WantedCheatEv";
	_callMap[59] = "_ZN6CCheat9RiotCheatEv";
	_callMap[60] = "_ZN6CCheat13FunhouseCheatEv";
	_callMap[61] = "_ZN6CCheat15AdrenalineCheatEv";
	_callMap[62] = "_ZN6CCheat12DrivebyCheatEv";
	_callMap[63] = "_ZN6CCheat24CountrysideInvasionCheatEv";
	_callMap[64] = "_ZN6CCheat12StaminaCheatEv";
	_callMap[65] = "_ZN6CCheat17WeaponSkillsCheatEv";
	_callMap[66] = "_ZN6CCheat18VehicleSkillsCheatEv";
	_callMap[67] = "_ZN6CCheat11ApacheCheatEv";
	_callMap[68] = "_ZN6CCheat9QuadCheatEv";
	_callMap[69] = "_ZN6CCheat11TankerCheatEv";
	_callMap[70] = "_ZN6CCheat10DozerCheatEv";
	_callMap[71] = "_ZN6CCheat15StuntPlaneCheatEv";
	_callMap[72] = "_ZN6CCheat17MonsterTruckCheatEv";
	_callMap[73] = "_ZN6CCheat15xboxHelperCheatEv";
}

void CCheatGTASA::initCheat(void* dll)
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

void CCheatGTASA::callFunByFlag()
{
	std::map<string, CheatUnit>::iterator item;
	for (item = _cheatData.begin(); item != _cheatData.end(); item++)
	{
		if (item->second.bCall)
		{
			LOGE("replaceGameProcess %s %d\n", item->second.strCheatName, item->second.bCall);
			(item->second.pfunCheat)();
			item->second.bCall = false;
		}
	}
}
