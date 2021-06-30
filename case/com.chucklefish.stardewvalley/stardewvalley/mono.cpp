#include "mono.h"
#include "common.h"

#define _FROM_FILE_
#ifdef _FROM_FILE_
	//#include "data.h"	4596224
	#include "data2.h"
#endif

static char* _pDllBuffer = NULL;
static long _nDllSize = 5725696;//4596224;
static MonoThread* _pDllImage = NULL;
static bool _bInit = false;
static MonoThread* mono_selfthread = NULL;
static int _nKeepTime = 0;
static int _nKeepWaterCanMax = 0;
static int _nDailyLuckMax = 0;
static int _nStaminaKeepMax = 0;
static int _nHealthKeepMax = 0;

static mono_object_to_string_t mono_object_to_string = NULL;
static mono_string_to_utf8_t mono_string_to_utf8 = NULL;
static mono_object_unbox_t mono_object_unbox = NULL;
static mono_object_new_t mono_object_new = NULL;
static mono_method_get_class_t mono_method_get_class = NULL;
static mono_domain_get_t mono_domain_get = NULL;
 
static mono_class_vtable_t mono_class_vtable = NULL;
static mono_class_get_field_from_name_t mono_class_get_field_from_name = NULL;
static mono_field_static_get_value_t mono_field_static_get_value = NULL;
static mono_field_static_set_value_t mono_field_static_set_value = NULL;
static mono_field_get_value_t mono_field_get_value = NULL;
static mono_field_get_value_object_t mono_field_get_value_object = NULL;
static mono_field_set_value_t mono_field_set_value = NULL;
static mono_runtime_object_init_t mono_runtime_object_init = NULL;
static mono_class_get_fields_t mono_class_get_fields = NULL;
static mono_field_get_name_t mono_field_get_name = NULL;
static mono_field_get_flags_t mono_field_get_flags = NULL;
static mono_object_new_alloc_specific_t mono_object_new_alloc_specific = NULL;
static mono_method_get_object_t mono_method_get_object = NULL;
static mono_add_internal_call_t mono_add_internal_call = NULL;
 
static mono_get_root_domain_t mono_get_root_domain = NULL;
static mono_thread_attach_t mono_thread_attach = NULL;
static mono_thread_detach_t mono_thread_detach = NULL;
static mono_image_open_from_data_t mono_image_open_from_data = NULL;
static mono_assembly_load_from_full_t mono_assembly_load_from_full = NULL;
static mono_assembly_get_image_t mono_assembly_get_image = NULL;
static mono_class_from_name_t mono_class_from_name = NULL;
static mono_class_get_method_from_name_t mono_class_get_method_from_name = NULL;
static mono_runtime_invoke_t mono_runtime_invoke = NULL;

void *(*MONO_RUNTIME_INVOKE)(void* method, void* obj, void** params, void** exc);
void* mono_runtime_invoke_replace(void *method, void *obj, void **params, void **exc) {
	
	LOGE("MONO_RUNTIME_INVOKE\n");
	return MONO_RUNTIME_INVOKE(method, obj, params, exc);
}


int StardewValley_Tools_WateringCan_get_WaterLeft(const WateringCan* wateringCan)
{
	LOGE("StardewValley_Tools_WateringCan_get_WaterLeft begin\n");
	return wateringCan->waterLeft;
	LOGE("StardewValley_Tools_WateringCan_get_WaterLeft end\n");
} 

static void init_mono_function(unsigned long baseAddr)
{
	long module_start_addr = baseAddr;
 
	mono_object_to_string = (mono_object_to_string_t)(module_start_addr + 0x1D9DA8);
	mono_string_to_utf8 = (mono_string_to_utf8_t)(module_start_addr + 0x1D8B14);
	mono_object_unbox = (mono_object_unbox_t)(module_start_addr + 0x1CC34C);
	mono_object_new = (mono_object_new_t)(module_start_addr + 0x1D5DB4);
	mono_method_get_class = (mono_method_get_class_t)(module_start_addr + 0x1A1FBC);
	mono_domain_get = (mono_domain_get_t)(module_start_addr + 0x15582C);
	
	mono_class_vtable = (mono_class_vtable_t)(module_start_addr + 0x1CFB9C);
	mono_class_get_field_from_name = (mono_class_get_field_from_name_t)(module_start_addr + 0x1629A8);
	mono_field_static_get_value = (mono_field_static_get_value_t)(module_start_addr + 0x1D2728);
	mono_field_static_set_value = (mono_field_static_set_value_t)(module_start_addr + 0x1D15BC);
	mono_field_get_value = (mono_field_get_value_t)(module_start_addr + 0x1D1700);
	mono_field_get_value_object = (mono_field_get_value_object_t)(module_start_addr + 0x1D17BC);
	mono_field_set_value = (mono_field_set_value_t)(module_start_addr + 0x1D1548);
	mono_runtime_object_init = (mono_runtime_object_init_t)(module_start_addr + 0x1CBFF0);
	mono_class_get_fields = (mono_class_get_fields_t)(module_start_addr + 0x165998);
	mono_field_get_name = (mono_field_get_name_t)(module_start_addr + 0x162A9C);
	mono_field_get_flags = (mono_field_get_flags_t)(module_start_addr + 0x1661DC);
	mono_object_new_alloc_specific = (mono_object_new_alloc_specific_t)(module_start_addr + 0x1D6428);
	mono_method_get_object = (mono_method_get_object_t)(module_start_addr + 0x20BEFC);
	mono_add_internal_call = (mono_add_internal_call_t)(module_start_addr + 0x196540);
 
	mono_get_root_domain = (mono_get_root_domain_t)(module_start_addr + 0x15581C);
	mono_thread_attach = (mono_thread_attach_t)(module_start_addr + 0x1F4BC8);
	mono_thread_detach = (mono_thread_detach_t)(module_start_addr + 0x1F5448);
	mono_image_open_from_data = (mono_image_open_from_data_t)(module_start_addr + 0x199B1C);
	mono_assembly_load_from_full = (mono_assembly_load_from_full_t)(module_start_addr + 0x15B070);
	mono_assembly_get_image = (mono_assembly_get_image_t)(module_start_addr + 0x15E670);
	mono_class_from_name = (mono_class_from_name_t)(module_start_addr + 0x163F50);
	mono_class_get_method_from_name = (mono_class_get_method_from_name_t)(module_start_addr + 0x1664AC);
	mono_runtime_invoke = (mono_runtime_invoke_t)(module_start_addr + 0x1D1068);
	
	//mono_thread_attach(mono_get_root_domain());
}

static void init_mono_image()
{
#ifndef _FROM_FILE_
	FILE * pFile = fopen("/data/local/tmp/StardewValley.dll", "rb");
	if (!pFile){
		LOGE("open dll failure\n");
		return;
	}
		
	fseek(pFile, 0, SEEK_END);
	_nDllSize = ftell(pFile);//4596224
	LOGE("dll size: %d\n", _nDllSize);
	rewind(pFile);
	_pDllBuffer = (char *)malloc(sizeof(char)*_nDllSize);
	fread(_pDllBuffer, 1, _nDllSize, pFile);
	fclose(pFile);
#else
	_pDllBuffer = g_dllData;
#endif

	MonoImageOpenStatus status;
	_pDllImage = mono_image_open_from_data(_pDllBuffer, _nDllSize, 1, &status);
	MonoAssembly* assembly = mono_assembly_load_from_full(_pDllImage, "UNUSED", &status, 0);
	_pDllImage = mono_assembly_get_image(assembly);
}

void cheat_init(unsigned long baseAddr)
{
	if (_bInit)
	{
		LOGE("cheat_init have done\n");
		return;
	}		
	_bInit = true;
	
	init_mono_function(baseAddr);
	
	mono_selfthread = mono_thread_attach(mono_get_root_domain());//call mono_thread_attach is needed
	/*
遇到的问题：
	EXPORT_FUNC void doCheat(int arg0, int arg1, int arg2)
	{
		cheat_init(_baseAddr);	attach到线程
		//process_cheat_safe(arg0, arg1);	最后集成到va里面的时候，开启cheat和do cheat是在一个线程
		
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
		pthread_create(&cheat_thread, &attr, threadProcCheat, NULL);//只是为了测试用的
		后面开启cheat是在线程threadProcCheat
		因此会出现偶然崩溃
	}
	*/
	
	init_mono_image();
}

//StardewValley.Game1	public static Game1 game1;
MonoObject* get_game_player()
{
	void* pClassGame1 = mono_class_from_name(_pDllImage, "StardewValley", "Game1");
	void* methodGet_player = NULL;
	//LOGE("get_game_player\n");
	methodGet_player = mono_class_get_method_from_name(pClassGame1, "get_player", 0);
	return mono_runtime_invoke(methodGet_player, NULL, NULL, NULL);
}

void cheat_newSeason()
{
	LOGE("cheat_newSeason begin\n");
	MonoClass* pClass = mono_class_from_name(_pDllImage, "StardewValley", "Game1");
	if (!pClass)	LOGE("pClass null\n");
	LOGE("cheat_newSeason 1\n");
	MonoMethod* method = mono_class_get_method_from_name(pClass, "newSeason", 0);
	if (!method)	LOGE("method null\n");
	LOGE("cheat_newSeason 2\n");
	/* MonoObject* exception = NULL;
	mono_runtime_invoke(method, NULL, NULL, &exception);
	LOGE("cheat_newSeason 3 %p\n", method);
	if (exception) 
	{
		LOGE ("An exception was thrown in Fail ()\n");
	} */
	mono_runtime_invoke(method, NULL, NULL, NULL);
	LOGE("cheat_newSeason end\n");
}

void cheat_warpHome()
{
	LOGE("cheat_warpHome begin\n");
	MonoClass* pClass = mono_class_from_name(_pDllImage, "StardewValley", "Game1");
	if (!pClass)	LOGE("pClass null\n");
	MonoMethod* method = mono_class_get_method_from_name(pClass, "warpHome", 0);
	if (!method)	LOGE("method null\n");
	mono_runtime_invoke(method, NULL, NULL, NULL);
	LOGE("cheat_warpHome end\n");
}

/* 
getFortuneForecast getWeatherForecast两个函数使用的变量都是static的
使用mono_object_new，可以获取到当前的Forecast？ 
public static string currentSeason = "spring";
public static double dailyLuck = 0.001;
public static LocalizedContentManager content;
public static int weatherForTomorrow; 
*/
void cheat_weatherFortuneForecast(int index)
{
	LOGE("cheat_weatherFortuneForecast begin\n");
	MonoClass* pClass = mono_class_from_name(_pDllImage, "StardewValley", "Game1");
	MonoMethod* methodShowMsg = mono_class_get_method_from_name(pClass, "showGlobalMessage", 1);//static function with args
	if (!methodShowMsg)	LOGE("methodShowMsg null\n");
	
	MonoClass* pClassTV = mono_class_from_name(_pDllImage, "StardewValley.Objects", "TV");
	MonoMethod* methodForecast = NULL;
	if (1 == index)
		methodForecast = mono_class_get_method_from_name(pClassTV, "getFortuneForecast", 0);//not static virtual function without args
	else
		methodForecast = mono_class_get_method_from_name(pClassTV, "getWeatherForecast", 0);
	if (!methodForecast)	LOGE("methodForecast null\n");

	MonoObject *pObj = NULL;
	pObj = mono_object_new(mono_domain_get(), pClassTV/*mono_method_get_class(methodForecast)*/);
	void* obj = mono_runtime_invoke(methodForecast,pObj, NULL, NULL);//MonoString*
	if (!obj)	LOGE("getWeatherForecast return null\n");
	char* str = mono_string_to_utf8(obj);
	LOGE("###### forecast: %s\n", str);
	void *arg_ptrs[1];
	arg_ptrs[0] = obj;
	mono_runtime_invoke(methodShowMsg, NULL, arg_ptrs, NULL);
	LOGE("cheat_weatherFortuneForecast end\n");
}

void cheat_time(int index)
{
	LOGE("cheat_time begin\n");
	MonoClass* pClass = mono_class_from_name(_pDllImage, "StardewValley", "Game1");
	MonoVTable* vtable = mono_class_vtable(mono_domain_get(), pClass);
	if (vtable == NULL) LOGE("vtable == NULL\n");
	MonoClassField* field = mono_class_get_field_from_name(pClass, "timeOfDay");
	if (field == NULL) LOGE("field == NULL\n");
	int nTimeOfDay;
	mono_field_static_get_value(vtable, field, &nTimeOfDay);
	
	int nHour;
	int nMinute;
	if (1 == index)
	{
		nHour = nTimeOfDay / 100;
		nMinute = nTimeOfDay - nHour * 100;

		nMinute += 30;
		if (nMinute >= 60)
		{
			nMinute = nMinute - 60;
			nHour += 1;
			if (nHour >= 26)
			{
				nHour = 26;
				nMinute = 0;
			}
		}
		nTimeOfDay = nHour * 100 + nMinute;
	}
	else
	{
		nHour = nTimeOfDay / 100;
		nMinute = nTimeOfDay - nHour * 100;

		nMinute -= 30;
		if (nMinute < 0)
		{
			nHour -= 1;
			nMinute += 60;
			if (nHour < 6)
			{
				nHour = 6;
				nMinute = 0;
			}
		}
		nTimeOfDay = nHour * 100 + nMinute;
	}
	
	mono_field_static_set_value(vtable, field, &nTimeOfDay);
	LOGE("time %d", index);
	LOGE("cheat_time end\n");
}

static int g_nCurrentTime = 0;
void cheat_time_get()
{
	LOGE("cheat_time_get begin\n");
	MonoClass* pClass = mono_class_from_name(_pDllImage, "StardewValley", "Game1");
	MonoVTable* vtable = mono_class_vtable(mono_domain_get(), pClass);
	if (vtable == NULL) LOGE("vtable == NULL\n");
	MonoClassField* field = mono_class_get_field_from_name(pClass, "timeOfDay");//static var
	if (field == NULL) LOGE("field == NULL\n");
	int nTimeOfDay;
	mono_field_static_get_value(vtable, field, &nTimeOfDay);
	g_nCurrentTime = nTimeOfDay;
	LOGE("time %d", g_nCurrentTime);
	LOGE("cheat_time_get end\n");
}

void cheat_time_keep()
{
	if (0 == _nKeepTime)
		return;
	//LOGE("cheat_time_keep begin\n");
	MonoClass* pClass = mono_class_from_name(_pDllImage, "StardewValley", "Game1");
	MonoVTable *vtable = mono_class_vtable(mono_domain_get(), pClass);
	if (vtable == NULL) LOGE("vtable == NULL\n");
	MonoClassField* field = mono_class_get_field_from_name(pClass, "timeOfDay");
	if (field == NULL) LOGE("field == NULL\n");
	int nTimeOfDay;
	nTimeOfDay = g_nCurrentTime;
	mono_field_static_set_value(vtable, field, &nTimeOfDay);
	//LOGE("time %d", nTimeOfDay);
	//LOGE("cheat_time_keep end\n");
}

void output_fields(MonoClass* klass)
{
	MonoClassField* field;
	void* iter = NULL;//gpointer iter = NULL
	
	while ((field = mono_class_get_fields(klass, &iter))) 
	{
		LOGE ("Field: %s, flags 0x%x\n", mono_field_get_name(field), mono_field_get_flags(field));
	}	
	/*
		Field: waterCanMax, flags 0x6	
		Field: waterLeft, flags 0x1
	*/
}

void cheat_waterCanMax_keep()
{
	if (0 == _nKeepWaterCanMax)
		return;
	MonoObject* player = get_game_player();
	if (player == NULL)
		return;
	//LOGE("cheat_waterCanMax_get begin\n");
	MonoClass* pClass = mono_class_from_name(_pDllImage, "StardewValley", "Farmer");
	//output_fields(pClass);
	MonoMethod* method2 = NULL;
	method2 = mono_class_get_method_from_name(pClass, "get_CurrentTool", 0);
	if (method2 == NULL) LOGE("method2 == NULL\n");
	MonoObject* wateringCan = mono_runtime_invoke(method2, player, NULL, NULL);
	if (wateringCan == NULL)
	{
		LOGE("wateringCan == NULL\n");
		return;
	}		
	
	int waterMax = 0;
	int waterLeft = 0;
	MonoClass* pClassWateringCan = mono_class_from_name(_pDllImage, "StardewValley.Tools", "WateringCan");
	MonoClassField *field = mono_class_get_field_from_name(pClassWateringCan, "waterCanMax");//not static var
	if (field == NULL) LOGE("field == NULL\n"); 
	MonoClassField *field2 = mono_class_get_field_from_name(pClassWateringCan, "waterLeft");//not static var
	if (field2 == NULL) LOGE("field2 == NULL\n"); 
	mono_field_get_value(wateringCan, field, &waterMax);
	//LOGE("waterMax %d\n", waterMax);
	mono_field_get_value(wateringCan, field2, &waterLeft);
	//LOGE("waterLeft %d\n", waterLeft);
	mono_field_set_value(wateringCan, field2, &waterMax);
	
	/* MonoMethod* method2 = NULL;
	LOGE("aaa\n");
	method2 = mono_class_get_method_from_name(pClass, "set_WaterLeft", 1);
	if (method2 == NULL) LOGE("method2 == NULL\n");
	MonoReflectionMethod* rMonoReflectionMethod = NULL;
	rMonoReflectionMethod = mono_method_get_object(mono_domain_get(), method2, pClass);
	if (!rMonoReflectionMethod)	LOGE("mono_method_get_object return null\n");
	*/
	//LOGE("cheat_waterCanMax_get end\n");
}

void cheat_dailyLuckMax_keep()
{
	if (0 == _nDailyLuckMax)
		return;
	LOGE("dailyLuckMax begin\n");
	MonoClass* pClass = mono_class_from_name(_pDllImage, "StardewValley", "Game1");
	if (pClass == NULL) LOGE("pClass == NULL\n");
	MonoVTable* vtable = mono_class_vtable(mono_domain_get(), pClass);
	if (vtable == NULL) LOGE("vtable == NULL\n");
	MonoClassField* field = mono_class_get_field_from_name(pClass, "dailyLuck");
	if (field == NULL) LOGE("field == NULL\n");
/* 	for (int i = 0; i < 60; i++)
	{
		LOGE("mono_class_get_field_from_name: %d\n", i);
		field = mono_class_get_field_from_name(pClass, "dailyLuck");
		if (field != NULL)
		{
			LOGE("get it\n");
			break;
		}
	} */
	double nDailyLuckMax;
	nDailyLuckMax = 0.10000000149011612;
	if (field == NULL)
	{
		LOGE("abort cheat_dailyLuckMax_keep\n");
		return;
	}
	mono_field_static_set_value(vtable, field, &nDailyLuckMax);
	LOGE("dailyLuckMax end\n");
}

void cheat_stamina_max_keep()
{
	if (_nStaminaKeepMax == 0)
		return;
	MonoObject* player = get_game_player();
	if (player == NULL)
		return;
	LOGE("cheat_stamina_max begin\n");
	/* void* pClassGame1 = mono_class_from_name(_pDllImage, "StardewValley", "Game1");
	void* methodGet_player = NULL;
	LOGE("aaa\n");
	methodGet_player = mono_class_get_method_from_name(pClassGame1, "get_player", 0);
	MonoObject* ret = mono_runtime_invoke(methodGet_player, NULL, NULL, NULL); */
	
	
	void* pClassFarmer = mono_class_from_name(_pDllImage, "StardewValley", "Farmer");
	
	/* void *pObj = NULL;//MonoObject*
	pObj = mono_object_new(mono_domain_get(), pClassFarmer);
	if (pObj == NULL) LOGE("pObj == NULL\n");
	mono_runtime_object_init (pObj); */
	
	void* method = NULL;
	LOGE("aaa\n");
	method = mono_class_get_method_from_name(pClassFarmer, "set_stamina", 1);
	if (method == NULL) LOGE("method == NULL\n");
	void *arg_ptrs[1];
	float stamina = 270;
	arg_ptrs[0] = &stamina;
	LOGE("bbb\n");
	mono_runtime_invoke(method, player, arg_ptrs, NULL);
	
	LOGE("cheat_stamina_max end\n");
}

void cheat_money_max()
{
	MonoObject* player = get_game_player();
	if (player == NULL)
		return;
	LOGE("cheat_money_max begin\n");
	void* pClassFarmer = mono_class_from_name(_pDllImage, "StardewValley", "Farmer");
	
	void* method_get_money = NULL;
	method_get_money = mono_class_get_method_from_name(pClassFarmer, "get_money", 0);
	if (method_get_money == NULL) LOGE("method_get_money == NULL\n");
	void* retval = mono_runtime_invoke(method_get_money, player, NULL, NULL);
	if (!retval)	LOGE("get_money return null\n");
	LOGE("invoke get_money\n");
	int res = {0};
	res = *(int *) mono_object_unbox(retval);
	LOGE("get money %d\n", res);
	
	void* method_set_money = NULL;
	method_set_money = mono_class_get_method_from_name(pClassFarmer, "set_money", 1);
	void *arg_ptrs[1];
	int money = 99999999;
	arg_ptrs[0] = &money;
	LOGE("bbb\n");
	mono_runtime_invoke(method_set_money, player, arg_ptrs, NULL);
	
	LOGE("cheat_money_max end\n");
}

void cheat_Level_max(int which)
{
	MonoObject* player = get_game_player();
	if (player == NULL)
		return;
	LOGE("cheat_fishingLevel_max begin\n");
	void* pClassFarmer = mono_class_from_name(_pDllImage, "StardewValley", "Farmer");
	
	void *methodLevel = NULL;
	switch (which)
	{
		case 0:
		{
			methodLevel = mono_class_get_method_from_name(pClassFarmer, "set_FarmingLevel",1);
		}
		break;
		case 1:
		{
			methodLevel = mono_class_get_method_from_name(pClassFarmer, "set_FishingLevel",1);
		}
		break;
		case 2:
		{
			methodLevel = mono_class_get_method_from_name(pClassFarmer, "set_ForagingLevel",1);
		}
		break;
		case 3:
		{
			methodLevel = mono_class_get_method_from_name(pClassFarmer, "set_MiningLevel",1);
		}
		break;
		case 4:
		{
			methodLevel = mono_class_get_method_from_name(pClassFarmer, "set_CombatLevel",1);
		}
		break;
		case 5:
		{
			methodLevel = mono_class_get_method_from_name(pClassFarmer, "set_LuckLevel",1);
		}
		break;
	}
	if (methodLevel == NULL) LOGE("methodLevel == NULL\n");
	
	void *arg_ptrs[1];
	int level = 10;
	arg_ptrs[0] = &level;
	LOGE("bbb\n");
	mono_runtime_invoke(methodLevel, player, arg_ptrs, NULL);
	
	LOGE("cheat_fishingLevel_max end\n");
}

void cheat_movement_speed()
{
	MonoObject* player = get_game_player();
	if (player == NULL)
		return;
	LOGE("cheat_movement_speed begin\n");
	//movementSpeed
	//runningSpeed
	//walkingSpeed
	
	MonoClass* pClassFarmer = mono_class_from_name(_pDllImage, "StardewValley", "Farmer");
	
	MonoMethod* method = NULL;
	LOGE("aaa\n");
	method = mono_class_get_method_from_name(pClassFarmer, "getMovementSpeed", 0);
	if (method == NULL) LOGE("method == NULL\n");
	LOGE("bbb\n");
	void* retval = mono_runtime_invoke(method, player, NULL, NULL);
	if (!retval)	LOGE("getMovementSpeed return null\n");
	LOGE("invoke getMovementSpeed\n");
	float res = {0};
	res = *(float *) mono_object_unbox(retval);
	LOGE("getMovementSpeed: %lf\n", res);
	
	MonoClass* pClassFarmerCharacter = mono_class_from_name(_pDllImage, "StardewValley", "Character");
	if (pClassFarmerCharacter == NULL)	LOGE("pClassFarmerCharacter == NULL\n");
	int speed = 0;
	MonoMethod* methodGetSpeed = mono_class_get_method_from_name(pClassFarmerCharacter, "get_speed", 0);
	if (methodGetSpeed == NULL) LOGE("methodGetSpeed == NULL\n");
	void* retvalSpeed = mono_runtime_invoke(methodGetSpeed, player, NULL, NULL);
	LOGE("invoke get_speed\n");
	int args = {0};
	args = *(int *) mono_object_unbox(retvalSpeed);
	LOGE("get_speed: %d\n", args);
	
	MonoMethod* methodSetSpeed = mono_class_get_method_from_name(pClassFarmerCharacter, "set_speed", 1);
	if (methodSetSpeed == NULL) LOGE("methodSetSpeed == NULL\n");
	void *arg_ptrs[1];
	int tmpSpeed = 3;//args*2;
	arg_ptrs[0] = &tmpSpeed;
	mono_runtime_invoke(methodSetSpeed, player, arg_ptrs, NULL);
	
	
	/* MonoClassField *field = mono_class_get_field_from_name(pClassFarmerCharacter, "speed");
	if (field == NULL) LOGE("field == NULL\n"); 
	mono_field_get_value(player, field, &speed);
	LOGE("get speed %d\n", speed);
	speed *= 2;
	mono_field_set_value(player, field, &speed); */
	
	LOGE("cheat_movement_speed end\n");
}

void cheat_health_max_keep()
{
	if (_nHealthKeepMax == 0)
		return;
	MonoObject* player = get_game_player();
	if (player == NULL)
		return;
	LOGE("cheat_health_max begin\n");
	MonoClass* pClassFarmer = mono_class_from_name(_pDllImage, "StardewValley", "Farmer");

	MonoClassField *fieldMaxHealth = mono_class_get_field_from_name(pClassFarmer, "maxHealth");
	if (fieldMaxHealth == NULL) LOGE("fieldMaxHealth == NULL\n"); 
	int maxHealth = 0;
	mono_field_get_value(player, fieldMaxHealth, &maxHealth);
	LOGE("maxHealth %d\n", maxHealth); 
	
	MonoClassField *fieldHealth = mono_class_get_field_from_name(pClassFarmer, "health");
	if (fieldHealth == NULL) LOGE("fieldHealth == NULL\n"); 
	mono_field_set_value(player, fieldHealth, &maxHealth);
	
	LOGE("cheat_health_max end\n");
}

void cheat_attack_more()
{
	MonoObject* player = get_game_player();
	if (player == NULL)
		return;
	LOGE("cheat_attack_more begin\n");
	MonoClass* pClassFarmer = mono_class_from_name(_pDllImage, "StardewValley", "Farmer");

	MonoClassField *fieldAttack = mono_class_get_field_from_name(pClassFarmer, "attack");
	if (fieldAttack == NULL) LOGE("fieldAttack == NULL\n"); 
	int attack = 0;
	mono_field_get_value(player, fieldAttack, &attack);
	LOGE("attack %d\n", attack); 
	
	int attackMore = attack + 100;
	mono_field_set_value(player, fieldAttack, &attackMore);
	LOGE("cheat_attack_more end\n");
}

void cheat_maxItems_more(int arg)
{
	MonoObject* player = get_game_player();
	if (player == NULL)
		return;
	LOGE("cheat_maxItems_more begin\n");
	MonoClass* pClassFarmer = mono_class_from_name(_pDllImage, "StardewValley", "Farmer");
	MonoMethod* method = NULL;
	MonoMethod* methodInc = NULL;
	method = mono_class_get_method_from_name(pClassFarmer, "get_MaxItems", 0);
	if (method == NULL) LOGE("method == NULL\n");
	methodInc = mono_class_get_method_from_name(pClassFarmer, "increaseBackpackSize", 1);//set_MaxItems
	if (methodInc == NULL) LOGE("methodInc == NULL\n");
	void* retval = mono_runtime_invoke(method, player, NULL, NULL);
	if (!retval)	LOGE("get_MaxItems return null\n");
	int res = {0};
	res = *(int *) mono_object_unbox(retval);
	LOGE("get_MaxItems: %d\n", res);
	
	void *arg_ptrs[1];
	int item = 12;
	/* if (arg == 1)
	{
		item = 72;
	}
	else
	{
		item = 36;
	} */
	arg_ptrs[0] = &item;
	mono_runtime_invoke(methodInc, player, arg_ptrs, NULL);
	LOGE("cheat_maxItems_more end\n");
}

void cheat_ToolMaxLevel()
{
	MonoObject* player = get_game_player();
	if (player == NULL)
		return;
	LOGE("cheat_ToolMaxLevel begin\n");
	MonoClass* pClassFarmer = mono_class_from_name(_pDllImage, "StardewValley", "Farmer");
	MonoMethod* method2 = NULL;
	method2 = mono_class_get_method_from_name(pClassFarmer, "get_CurrentTool", 0);
	if (method2 == NULL) LOGE("method2 == NULL\n");
	MonoObject* toolObj = mono_runtime_invoke(method2, player, NULL, NULL);
	if (toolObj == NULL)
	{
		LOGE("toolObj == NULL\n");
		return;
	}		
	
	MonoClass* pClassTool = mono_class_from_name(_pDllImage, "StardewValley", "Tool");
	MonoMethod* methodGet = NULL;
	MonoMethod* methodSet = NULL;
	methodGet = mono_class_get_method_from_name(pClassTool, "get_UpgradeLevel", 0);
	if (methodGet == NULL) LOGE("methodGet == NULL\n");
	methodSet = mono_class_get_method_from_name(pClassTool, "set_UpgradeLevel", 1);
	if (methodSet == NULL) LOGE("methodSet == NULL\n");
	void* retval = mono_runtime_invoke(methodGet, toolObj, NULL, NULL);
	if (!retval)	LOGE("get_UpgradeLevel return null\n");
	int res = {0};
	res = *(int *) mono_object_unbox(retval);
	LOGE("get_UpgradeLevel: tool level %d\n", res);
	
	void *arg_ptrs[1];
	int level = 4;
	arg_ptrs[0] = &level;
	mono_runtime_invoke(methodSet, toolObj, arg_ptrs, NULL);
	
	LOGE("cheat_ToolMaxLevel end\n");
}

void cheat_process(int index, int args)
{
	LOGE("cheat_process %d, %d\n", index, args);
	switch (index)
	{
		case 1:
		{
			cheat_newSeason();
		}
		break;
		case 2:
		{
			cheat_warpHome();
		}
		break;
		case 3:
		{
			cheat_weatherFortuneForecast(args);
		}
		break;
		case 4:
		{
			cheat_time(args);
		}
		break;
		case 5:
		{
			cheat_time_get();
			_nKeepTime = args;
		}
		break;
		case 6:
		{
			_nKeepWaterCanMax = args;
		}
		break;
		case 7:
		{
			_nDailyLuckMax = args;
		}
		break;
		case 8:
		{
			_nStaminaKeepMax = args;
		}
		break;
		case 9:
		{
			cheat_movement_speed();
		}
		break;
		case 10:
		{
			_nHealthKeepMax = args;
		}
		break;
		case 11:
		{
			cheat_money_max();
		}
		break;
		case 12:
		{
			cheat_Level_max(args);
		}
		break;
		case 13:
		{
			cheat_attack_more();
		}
		break;
		case 14:
		{
			cheat_maxItems_more(args);
		}
		break;
		case 15:
		{
			cheat_ToolMaxLevel();
		}
		break;
	}
}

void exit_mono()
{
	mono_thread_detach(mono_selfthread);
}

void cheatTest(unsigned long baseAddr)
{
	long module_start_addr = baseAddr;
 
	mono_object_to_string_t mono_object_to_string = (mono_object_to_string_t)(module_start_addr + 0x1D9DA8);
	if (!mono_object_to_string)
		LOGE("mono_object_to_string null\n");
	mono_string_to_utf8_t mono_string_to_utf8 = (mono_string_to_utf8_t)(module_start_addr + 0x1D8B14);
	if (!mono_string_to_utf8)
		LOGE("mono_string_to_utf8 null\n");
	
	mono_object_unbox_t mono_object_unbox = (mono_object_unbox_t)(module_start_addr + 0x1CC34C);
	if (!mono_object_unbox)
		LOGE("mono_object_unbox null\n");
	mono_object_new_t mono_object_new = (mono_object_new_t)(module_start_addr + 0x1D5DB4);
	mono_method_get_class_t mono_method_get_class = (mono_method_get_class_t)(module_start_addr + 0x1A1FBC);
	mono_domain_get_t mono_domain_get = (mono_domain_get_t)(module_start_addr + 0x15582C);
	
	mono_class_vtable_t mono_class_vtable = (mono_class_vtable_t)(module_start_addr + 0x1CFB9C);
	mono_class_get_field_from_name_t mono_class_get_field_from_name = (mono_class_get_field_from_name_t)(module_start_addr + 0x1629A8);
	mono_field_static_get_value_t mono_field_static_get_value = (mono_field_static_get_value_t)(module_start_addr + 0x1D2728);
	mono_field_static_set_value_t mono_field_static_set_value = (mono_field_static_set_value_t)(module_start_addr + 0x1D15BC);
 
	mono_get_root_domain_t mono_get_root_domain = (mono_get_root_domain_t)(module_start_addr + 0x15581C);
	mono_thread_attach_t mono_thread_attach = (mono_thread_attach_t)(module_start_addr + 0x1F4BC8);
	mono_image_open_from_data_t mono_image_open_from_data = (mono_image_open_from_data_t)(module_start_addr + 0x199B1C);
	mono_assembly_load_from_full_t mono_assembly_load_from_full = (mono_assembly_load_from_full_t)(module_start_addr + 0x15B070);
	mono_assembly_get_image_t mono_assembly_get_image = (mono_assembly_get_image_t)(module_start_addr + 0x15E670);
	mono_class_from_name_t mono_class_from_name = (mono_class_from_name_t)(module_start_addr + 0x163F50);
	mono_class_get_method_from_name_t mono_class_get_method_from_name = (mono_class_get_method_from_name_t)(module_start_addr + 0x1664AC);
	mono_runtime_invoke_t mono_runtime_invoke = (mono_runtime_invoke_t)(module_start_addr + 0x1D1068);
	
	mono_thread_attach(mono_get_root_domain());
	
	FILE * pFile = fopen("/data/local/tmp/StardewValley.dll", "rb");
	if (!pFile){
		LOGE("open dll failure\n");
		return;
	}
		
	fseek(pFile, 0, SEEK_END);
	long lSize = ftell(pFile);//4596224
	LOGE("dll size: %d\n", lSize);
	rewind(pFile);
	char * buffer = (char *)malloc(sizeof(char)*lSize);
	fread(buffer, 1, lSize, pFile);
	fclose(pFile);
	
	LOGE("read dll finish\n");
	MonoImageOpenStatus status;
	void* image = mono_image_open_from_data(buffer, lSize, 1, &status);
	LOGE("ppp1\n");
	void* assembly = mono_assembly_load_from_full(image, "UNUSED", &status, 0);
	LOGE("ppp2\n");
	image = mono_assembly_get_image(assembly);
	LOGE("ppp3\n");
	void* pClass = mono_class_from_name(image, "StardewValley", "Game1");
	LOGE("ppp4\n");
	/*void* method = mono_class_get_method_from_name(pClass, "newSeason", 0);//newSeason warpHome 
	if (!method)
		LOGE("method null\n");
	LOGE("ppp5\n");
	mono_runtime_invoke(method, NULL, NULL, NULL);
	LOGE("ppp6\n");*/
	
	/*void *vtable = mono_class_vtable(mono_domain_get(), pClass);//直接改时间变量好像会crash 需要通过函数来改？
	if (vtable == NULL) LOGE("vtable == NULL\n");
	void *field = mono_class_get_field_from_name(pClass, "timeOfDay");
	if (field == NULL) LOGE("field == NULL\n");
	int nTimeOfDay;
	mono_field_static_get_value(vtable, field, &nTimeOfDay);
	nTimeOfDay += 30;
	mono_field_static_set_value(vtable, field, &nTimeOfDay);
	LOGE("time + 30");*/
	
	void* method3 = mono_class_get_method_from_name(pClass, "showGlobalMessage", 1);
	if (!method3)
		LOGE("method3 null\n");
	
	void* pClass2 = mono_class_from_name(image, "StardewValley.Objects", "TV");
	void* method2 = mono_class_get_method_from_name(pClass2, "getFortuneForecast", 0);//getWeatherForecast getFortuneForecast not static fun
	if (!method2)
		LOGE("method2 null\n");

	LOGE("ppp7\n");
	void *pThis;//MonoObject*
	pThis = mono_object_new(mono_domain_get(), mono_method_get_class(method2));
	void* obj = mono_runtime_invoke(method2, pThis, NULL, NULL);//MonoObject* 其实是MonoString*
	LOGE("ppp8\n");
	/*void* msg = mono_object_to_string(obj, NULL);//MonoString* 
	LOGE("ppp9\n");
	void* str = mono_string_to_utf8(msg);//char*
	LOGE("ppp10\n");*/
	//void* ggg = mono_object_unbox(obj);	// catch SIGSEGV when read or write mem
	//#define UNBOX_CHAR_PTR(x) (char *)mono_object_unbox(x)
	if (!obj)
		LOGE("getWeatherForecast return null\n");
	char* str = mono_string_to_utf8(obj);//obj  MonoString *???
	LOGE("###### weather: %s\n", str);
	
	LOGE("ppp9\n");
	void *arg_ptrs[1];
	arg_ptrs[0] = obj;// 显示不出来msg
	mono_runtime_invoke(method3, NULL, arg_ptrs, NULL);
	LOGE("ppp11\n");
	
	free(buffer);
}