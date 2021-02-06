#include <jni.h>
#include "xhook.h"
#include <android/log.h>
#include <stdio.h>
#include <aiquyou.h>
#include<string.h>
#include<time.h>

#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>
#include<dlfcn.h>

#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <assert.h>

#define TAG "NTAG"
#define LOG_E(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)
//using namespace std;
#include "inlinehook\inlineHook.h"

float speed = 1.0;
int symIndex;
char * passBaseDirect;

int reckeckCount;
jobject mContext;

jstring get_p(JNIEnv *env, jclass ywcls) {
	char ac[200] = "ACTIVITY";
	char act[200] = "Landroid/app/Activity;";
	jfieldID context_field_id = env->GetStaticFieldID(ywcls, ac, act);
	jobject context_field_object = env->GetStaticObjectField(ywcls,
			context_field_id);
	mContext = context_field_object;
	jclass context_class = env->GetObjectClass(context_field_object);
	char gpn[100] = "getPackageName";
	char gpns[100] = "()Ljava/lang/String;";
	jmethodID pkg_meth_id = env->GetMethodID(context_class, gpn, gpns);

	jstring pkg_object = static_cast<jstring>(env->CallObjectMethod(
			context_field_object, pkg_meth_id));
//	char *c_msg = (char*) env->GetStringUTFChars(pkg_object, 0);
//		LOG_E(c_msg);
	return pkg_object;
}

jstring get_s(JNIEnv *env, jclass ywcls, jstring p) {
	char act[100] = "ACTIVITY";
	char acts[100] = "Landroid/app/Activity;";
	jfieldID context_field_id = env->GetStaticFieldID(ywcls, act, acts);
	jobject context_field_object = env->GetStaticObjectField(ywcls,
			context_field_id);
	jclass context_class = env->GetObjectClass(context_field_object);
	char pm[50] = "getPackageManager";
	char amps[200] = "()Landroid/content/pm/PackageManager;";
	jmethodID pkg_mng_meth_id = env->GetMethodID(context_class, pm, amps);
	jobject pkg_mng_obj = env->CallObjectMethod(context_field_object,
			pkg_mng_meth_id);
	jclass pkg_mng_class = env->GetObjectClass(pkg_mng_obj);
	char gp[50] = "getPackageInfo";
	char ampi[200] = "(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;";
	jmethodID pkg_info_meth_id = env->GetMethodID(pkg_mng_class, gp, ampi);
	jobject pkg_info_object = env->CallObjectMethod(pkg_mng_obj,
			pkg_info_meth_id, p, 64);
	jclass pkg_info_class = env->GetObjectClass(pkg_info_object);
	char ss[30] = "signatures";
	char sss[100] = "[Landroid/content/pm/Signature;";
	jfieldID sign_field_id = env->GetFieldID(pkg_info_class, ss, sss);
	jobject sign_objects = env->GetObjectField(pkg_info_object, sign_field_id);
	jobjectArray sign_array = (jobjectArray) sign_objects;
	jsize size = env->GetArrayLength(sign_array);
	jobject current_sign_obj = env->GetObjectArrayElement(sign_array, 0);
	jclass current_sign_class = env->GetObjectClass(current_sign_obj);
	char ts[50] = "toCharsString";
	char tss[50] = "()Ljava/lang/String;";
	jmethodID sign_2_str_id = env->GetMethodID(current_sign_class, ts, tss);
	jstring sign = static_cast<jstring>(env->CallObjectMethod(current_sign_obj,
			sign_2_str_id));
//	char *c_msg = (char*) env->GetStringUTFChars(sign, 0);
//	LOG_E(c_msg);
	return sign;
}


void Java_com_longshao_aiquyouxi_AiQuYouXi_AiQuYouXis(JNIEnv *env, jclass ywcls,
		jfloat num) {

	jfloat aiquyouxiting = 1000.0f;

	if (num > 0 && num < 11 && num < aiquyouxiting) {
		speed = num;
	}
}

bool checkfile(char *a, JNIEnv *env) {
	if (a != NULL) {
		char st[100] = "3082037d30820265a003020102020434639c5a3";
		char mi[100] = "60355040a130b5869614d656e20535748593113";
		char en[100] = "ceff9c33d2cebc3af0a07394a782f1e0f140aa8";
		if (strstr(a, st) != NULL && strstr(a, mi) != NULL
				&& NULL != strstr(a, en)) {
			return true;
		}
	}
	jclass jcl = *(jclass *) dlsym(dlopen("libhack", 1), "hook_game");
	env->GetStaticFieldID(jcl, "hook_game_handler", "Ljava/lang/String;");
	return false;
}

bool checkAiQufile(char *a ,JNIEnv *env){
 	if(a != NULL){
 		char sty[100]= "3082036d30820255a0030201020204108686bb300d06092a864886f70";
 		char miy[100]= "56cdb91d4bb1ae3b89565eeeeaeaac6dbbd7085d37ec96f81699d";
 		char eny[100]= "6909016cc1505fd1149e3a00c5d6cb30a52a9d67c67696c9f4";
 		if(strstr(a,sty) != NULL && strstr(a,miy)!= NULL && NULL != strstr(a,eny)){
 			return true;
 		}
 	}
 	LOG_E("chekc Error");
 	jclass jcl = *(jclass *)dlsym(dlopen("libhack",1),"hook_game");
 	env->GetStaticFieldID(jcl,"hook_game_handler","Ljava/lang/String;");
 	return false;
 }

const int COS_NOR = 2;
const int COS_SPC = 5;
const int COS_SCALE = 1;
const int U3D_MONO = 5;
const int IL_SCRI = 5;
const int IL_CODE = 3;

const char * cocos2d_mornal[COS_NOR] = {
		"_ZN7cocos2d11CCScheduler6updateEf",
		"_ZN7cocos2d9Scheduler6updateEf"
};

const char * cocos2d_special[COS_SPC] = {
		"_ZN7cocos2d15CCActionManager6updateEf",
		"_ZN7cocos2d11CCScheduler4tickEf",
		"_ZN7cocos2d13ActionManager6updateEf",
		"_ZN7cocos2d5Speed4stepEf",
		"_ZN7cocos2d4Node8scheduleEMNS_3RefEFvfEf"

};


const char * u3d_mono[U3D_MONO] = {
		"mono_class_get_namespace",
		"mono_get_object_class",
		"mono_class_from_name",
		"mono_runtime_invoke",
		"mono_class_get_method_from_name"
};

const char * il2cpp_script[IL_SCRI] = {
		"il2cpp_runtime_invoke",
		"il2cpp_method_get_class",
		"il2cpp_class_get_image",
		"il2cpp_class_from_name",
		"il2cpp_class_get_method_from_name" };

const char * il2cpp_code[IL_CODE] = {
		"_ZN6il2cpp2vm7Runtime6InvokeEPK10MethodInfoPvPS5_PP15Il2CppException",
		"_ZN6il2cpp2vm5Class17GetMethodFromNameEP11Il2CppClassPKci",
		"_ZN6il2cpp2vm5Image13ClassFromNameEPK11Il2CppImagePKcS6_" };

//cocos


jint countUpdate = 0;

void (*COSNORMAL)(void *v, float dt);

void cos_normal(void *v, float dt) {
	dt = dt * speed;

	jfloat repeatTime= 0.0;

	while(repeatTime < speed){
		repeatTime = repeatTime+0.5;
		COSNORMAL(v, dt);
	}

	COSNORMAL(v, dt);
}

void cocosNormal(void * handler,const char *pathname_regex_str) {
	void *update;
	void *update2;

	update = dlsym(handler, cocos2d_mornal[0]);
	update2 = dlsym(handler, cocos2d_mornal[1]);
	if (update != NULL) {
		LOG_E("cocosNormal catch1");
			xhook_register(pathname_regex_str,cocos2d_mornal[0],(void *) &cos_normal,(void**) &COSNORMAL);
			xhook_refresh(0);
	} else if (update2 != NULL) {
		LOG_E("cocosNormal catch2");
		xhook_register(pathname_regex_str,cocos2d_mornal[1],(void *) &cos_normal,(void**) &COSNORMAL);
		xhook_refresh(0);
	} else {
		LOG_E("cocosNormal nocatch2");
	}
}



//cocos

void (*COSSPECIAL_ONE)(void *v, float dt);

void cos_special_one(void *v, float dt) {
	dt = dt * speed;
	COSSPECIAL_ONE(v, dt);
}

void *(*COSSPECIAL_TWO)(void *v, float dt);

void * cos_special_two(void *v, float dt) {
	dt = dt * speed;
	return COSSPECIAL_TWO(v, dt);
}

void *(*COSSPECIAL_THREE)(void *v, float dt);

void * cos_special_three(void *v, float dt) {
	dt = dt * speed;
	return COSSPECIAL_THREE(v, dt);
}

bool (*COSSPECIAL_FOUR)(void *__hidden, float radio);

bool cos_special_four(void *__hidden, float radio) {


	LOG_E("speed_step = %f",radio);

	radio =radio *speed;

	return COSSPECIAL_FOUR(__hidden,radio);
}

void *(*COSSPECIAL_FIVE)(void* envirenment, float delater);

void * cos_special_five(void * envirenment, float delater) {
	LOG_E("delater = %f",delater);

	return COSSPECIAL_FIVE(envirenment,delater);
}

void cocosSpecail(void * handler,const char *pathname_regex_str) {
	void *update;
	void *update2;
	void * update3;
	void * update4;
	void * update5;


	update = dlsym(handler, cocos2d_special[0]);
	update2 = dlsym(handler, cocos2d_special[1]);

	update3 = dlsym(handler, cocos2d_special[2]);

	update4 = dlsym(handler, cocos2d_special[3]);

	update5 = dlsym(handler,cocos2d_special[4]);

	if (update != NULL) {
		LOG_E("cocosSpecail catch1");
		xhook_register(pathname_regex_str,cocos2d_special[0],(void *) &cos_special_one,(void**) &COSSPECIAL_ONE);
		xhook_refresh(0);
	} else {
		LOG_E("cocosSpecail nocatch1");
	}
	if (update2 != NULL) {
		LOG_E("cocosSpecail catch 2");
		xhook_register(pathname_regex_str,cocos2d_special[1],(void *) &cos_special_two,(void**) &COSSPECIAL_TWO);
		xhook_refresh(0);

	} else {
		LOG_E("cocosSpecail nocatch 2");
	}

	if (update3 != NULL) {
		LOG_E("cocosSpecail catch 3");
		xhook_register(pathname_regex_str,cocos2d_special[2],(void *) &cos_special_three,(void**) &COSSPECIAL_THREE);
		xhook_refresh(0);

	} else {
		LOG_E("cocosSpecail nocatch 3");
	}

	if (update4 != NULL) {
		LOG_E("cocosSpecail catch 4");
		xhook_register(pathname_regex_str,cocos2d_special[3],(void *) &cos_special_four,(void**) &COSSPECIAL_FOUR);
		xhook_refresh(0);

	} else {
		LOG_E("cocosSpecail nocatch 4");
	}

	if (update5 != NULL) {
		LOG_E("cocosSpecail catch 5");
		xhook_register(pathname_regex_str,cocos2d_special[4],(void *) &cos_special_five,(void**) &COSSPECIAL_FIVE);
		xhook_refresh(0);
	} else {
		LOG_E("cocosSpecail nocatch 5");
	}
}

//u3d mono

typedef void *(*MONO_CLASS_GET_METHOD_FROM_NAME)(void *mono_class, char *name,
		int param_count);
MONO_CLASS_GET_METHOD_FROM_NAME mono_get_method_from_name;

typedef void * (*MONO_GET_OBJECT_CLASS)(void * mobject);
MONO_GET_OBJECT_CLASS mono_get_object_class;
typedef const char * (*MONO_CLASS_GET_NAME_SPACE)(void *mclass);
MONO_CLASS_GET_NAME_SPACE mono_class_get_namespace;

void *(*MONO_CLASS_FROM_NAME)(void *image, char *name_space, char *name);
void *mono_time_class;
bool mono_hook_one_switch;
int mono_count;
void *mono_time_scale_method;

char mono_ue[] = "UnityEngine";
char mono_time[] = "Time";
char mono_time_scale[] = "set_timeScale";

void* mono_class_from_name(void *image, char *name_space, char *name) {
	if (!strcmp(name_space, mono_ue) && !strcmp(name, mono_time)
			&& !mono_hook_one_switch) {
		mono_time_class = MONO_CLASS_FROM_NAME(image, name_space, name);
		mono_time_scale_method = mono_get_method_from_name(mono_time_class,
				mono_time_scale, 1);
		if (mono_time_scale_method != NULL) {
			mono_hook_one_switch = true;
		}
	}
	return MONO_CLASS_FROM_NAME(image, name_space, name);
}

void *(*MONO_RUNTIME_INVOKE)(void *method, void *obj, void **params,
		void **exc);

void* mono_runtime_invoke(void *method, void *obj, void **params, void **exc) {
	if (mono_hook_one_switch) {
		if (mono_count >= 49) {
			void *args[1];
			float scale = speed;
			args[0] = &scale;
			MONO_RUNTIME_INVOKE(mono_time_scale_method, NULL, args, NULL);
			mono_count = 0;
		}
		++mono_count;
	}
	return MONO_RUNTIME_INVOKE(method, obj, params, exc);
}

void u3dMono(void * handler,const char *pathname_regex_str) {

	mono_count = 0;
	mono_hook_one_switch = false;

	void *mono_class_from_name_sym;
	void *mono_runtime_invoke_sym;


	mono_class_from_name_sym = dlsym(handler, u3d_mono[2]);


	mono_runtime_invoke_sym = dlsym(handler, u3d_mono[3]);


	mono_get_method_from_name = (MONO_CLASS_GET_METHOD_FROM_NAME) dlsym(handler,
			u3d_mono[4]);

	if (mono_class_from_name_sym != NULL && mono_runtime_invoke_sym != NULL) {

		xhook_register(pathname_regex_str,u3d_mono[2],(void *) &cos_normal,(void**) &MONO_CLASS_FROM_NAME);
		xhook_refresh(0);
		xhook_register(pathname_regex_str,u3d_mono[3],(void *) &cos_normal,(void**) &MONO_RUNTIME_INVOKE);
		xhook_refresh(0);


		LOG_E("u3dMono catch");
	} else {
		LOG_E("u3dMono nocatch");
	}

}

//il2cpp script

typedef char *(*IL2CPP_METHOD_FROM_NAME)(void * class_object, char *name,
		int agrs);
typedef void *(*IL2CPP_METHOD_GET_CLASS)(void *method);
typedef void *(*IL2CPP_CLASS_GET_IMAGE)(void *class_object);
typedef void *(*IL2CPP_CLASS_FROM_NAME)(void *image, char *space, char *name);

IL2CPP_METHOD_FROM_NAME il2cpp_method_from_name;
IL2CPP_METHOD_GET_CLASS il2cpp_method_get_class;
IL2CPP_CLASS_GET_IMAGE il2cpp_class_get_image;
IL2CPP_CLASS_FROM_NAME il2cpp_class_from_name;

void * il2cpp_time_scale_method;
void *(*IL2CPP_RUN_TIME_INVOKE)(void *method, void *obj, void **params,
		void **exc);

void* il2cpp_run_time_invoke(void *method, void *obj, void **params,
		void **exc) {

	void *any_class = il2cpp_method_get_class(method);
	if (any_class != NULL) {
		void * image_name = il2cpp_class_get_image(any_class);
		if (image_name != NULL) {
			void *target_class = il2cpp_class_from_name(image_name,
					"UnityEngine", "Time");
			if (target_class != NULL) {
				il2cpp_time_scale_method = il2cpp_method_from_name(target_class,
						"set_timeScale", 1);
				if (il2cpp_time_scale_method != NULL) {
					void *args[1];
					float scale = speed;
					args[0] = &scale;
					IL2CPP_RUN_TIME_INVOKE(il2cpp_time_scale_method, NULL, args,
							NULL);
				} else {

				}
			} else {
			}
		} else {
		}
	} else {
	}
	return IL2CPP_RUN_TIME_INVOKE(method, obj, params, exc);
}

void il2cppScript(void * handler,const char *pathname_regex_str) {
	void * il2cpp_scr_run_time_invoke = dlsym(handler, il2cpp_script[0]);
	if (il2cpp_scr_run_time_invoke != NULL) {
		LOG_E("il2cppScript catch");
		il2cpp_method_get_class = (IL2CPP_METHOD_GET_CLASS) dlsym(handler,
				il2cpp_script[1]);
		il2cpp_class_get_image = (IL2CPP_CLASS_GET_IMAGE) dlsym(handler,
				il2cpp_script[2]);
		il2cpp_class_from_name = (IL2CPP_CLASS_FROM_NAME) dlsym(handler,
				il2cpp_script[3]);
		il2cpp_method_from_name = (IL2CPP_METHOD_FROM_NAME) dlsym(handler,
				il2cpp_script[4]);

				xhook_register(pathname_regex_str,il2cpp_script[0],(void *) &il2cpp_run_time_invoke,(void**) &IL2CPP_RUN_TIME_INVOKE);
				xhook_refresh(0);

	} else {
		LOG_E("il2cppScript nocatch");
	}

}

//il2cpp code

typedef char *(*IL2CPP_CODE_METHOD_FROM_NAME)(void * class_object, char *name,
		int agrs);

IL2CPP_CODE_METHOD_FROM_NAME il2cpp_code_method_from_name;

void *il2cpp_code_time_class;
void *il2cpp_code_time_scale_method;
void *(*IL2CPP_CODE_RUN_TIME_INVOKE)(void *method, void *obj, void **params,
		void **exc);

void* il2cpp_code_run_time_invoke(void *method, void *obj, void **params,
		void **exc) {
	if (il2cpp_code_time_scale_method != NULL) {
		void *args[1];
		float scale = speed;
		args[0] = &scale;
		IL2CPP_CODE_RUN_TIME_INVOKE(il2cpp_code_time_scale_method, NULL, args,
				NULL);
	}
	return IL2CPP_CODE_RUN_TIME_INVOKE(method, obj, params, exc);
}

void *(*IL2CPP_CODE_CLASS_FROM_NAME)(void const *image, char const *spaceName,
		char const *name);
void *il2cpp_code_class_from_name(void const *image, char const *spaceName,
		char const *name) {
	il2cpp_code_time_class = IL2CPP_CODE_CLASS_FROM_NAME(image, "UnityEngine",
			"Time");
	if (il2cpp_code_time_class != NULL) {
		il2cpp_code_time_scale_method = il2cpp_code_method_from_name(
				il2cpp_code_time_class, "set_timeScale", 1);
	}
	return IL2CPP_CODE_CLASS_FROM_NAME(image, spaceName, name);
}

void il2cppCode(void * handler,const char *pathname_regex_str) {
	void *run_time_invoke =
			dlsym(handler,
					il2cpp_code[0]);

	if (run_time_invoke != NULL) {
		LOG_E("il2cppCode catch");

		xhook_register(pathname_regex_str,il2cpp_code[0],(void *) &il2cpp_code_run_time_invoke,(void**) &IL2CPP_CODE_RUN_TIME_INVOKE);
		xhook_refresh(0);

		il2cpp_code_method_from_name = (IL2CPP_CODE_METHOD_FROM_NAME) dlsym(
				handler,
				il2cpp_code[1]);
		void * class_from_name_symble = dlsym(handler,
				il2cpp_code[2]);

				xhook_register(pathname_regex_str,il2cpp_code[2],(void *) &il2cpp_code_class_from_name,(void**) &IL2CPP_CODE_CLASS_FROM_NAME);
				xhook_refresh(0);

	} else {
		LOG_E("il2cppCode nocatch");
	}

}

//no engine

int64_t lastRealTime; //上一次的真实时间   //7048
int64_t lastExtraTime; //上一次添加了加速值之后的时间    //7040

int64_t nlastRealTime; //上一次的真实时间   //7048
int64_t nlastExtraTime; //上一次添加了加速值之后的时间    //7040
//加速

void gettimeofday_hook_up(struct timeval *tv, struct timezone *tz) {
	static int gCount1 = 1;
	int64_t time_curr = tv->tv_sec * 1000000LL + tv->tv_usec; //即时时间
	if (lastRealTime == 0) {
		lastRealTime = time_curr;
	}
	if (lastExtraTime == 0) {
		lastExtraTime = time_curr;
	}
	int64_t detaTime = time_curr - lastRealTime; //跟上次的时间差
	//当前时间加上要快进的时间
	uint64_t totalTime;
	if (detaTime == 0) {
		totalTime = lastExtraTime;
	} else {
		lastRealTime = time_curr; //记录当前时间，留着下次比较
		gCount1++;
		totalTime = ((double) speed - 1) * 250*gCount1 + detaTime + lastExtraTime;
		lastExtraTime = totalTime;
	}
	tv->tv_sec = (time_t) (totalTime / 1000000LL);
	tv->tv_usec = (suseconds_t) (totalTime % 1000000LL);

}

//减速
void gettimeofday_hook_down(struct timeval *tv, struct timezone *tz) {
	int64_t time_curr = tv->tv_sec * 1000000LL + tv->tv_usec; //即时时间
	if (lastRealTime == 0) {
		lastRealTime = time_curr;
	}
	if (lastExtraTime == 0) {
		lastExtraTime = time_curr;
	}
	int64_t detaTime = time_curr - lastRealTime; //跟上次的时间差
	int64_t totalTime; //当前时间加上要快进的时间
	if (detaTime == 0) {
		totalTime = lastExtraTime;
	} else {
		lastRealTime = time_curr; //记录当前时间，留着下次比较
		totalTime = detaTime - 300LL + lastExtraTime;
		lastExtraTime = totalTime;
	}
	tv->tv_sec = (time_t) (totalTime / 1000000LL);
	tv->tv_usec = (suseconds_t) (totalTime % 1000000LL);
}

void clock_gettime_hook_up(struct timespec *tp) {
	static int gCount = 1;
	int64_t time_curr = tp->tv_sec * 1000000000LL + tp->tv_nsec; //即时时间
	if (nlastRealTime == 0) {
		nlastRealTime = time_curr;
	}
	if (nlastExtraTime == 0) {
		nlastExtraTime = time_curr;
	}
	int64_t detaTime = time_curr - nlastRealTime; //跟上次的时间差
	int64_t totalTime; //当前时间加上要快进的时间
	if (detaTime == 0) {
		totalTime = nlastExtraTime;
	} else {
		nlastRealTime = time_curr; //记录当前时间，留着下次比较
		gCount++;
		totalTime = ((double) speed - 1) * 250*gCount + detaTime + nlastExtraTime;
		nlastExtraTime = totalTime;
	}
	tp->tv_sec = (time_t) (totalTime / 1000000000LL);
	tp->tv_nsec = (long) (totalTime % 1000000000LL);
}

void clock_gettime_hook_down(struct timespec *tp) {
	int64_t time_curr = tp->tv_sec * 1000000000LL + tp->tv_nsec; //即时时间
	int64_t detaTime = time_curr - nlastRealTime; //跟上次的时间差
	int64_t totalTime; //当前时间加上要快进的时间
	if (detaTime == 0) {
		totalTime = nlastExtraTime;
	} else {
		nlastRealTime = time_curr; //记录当前时间，留着下次比较
		if (nlastExtraTime == 0) {
			nlastExtraTime = time_curr;
		}
		totalTime = detaTime - 300LL + nlastExtraTime;
		nlastExtraTime = totalTime;
	}
	tp->tv_sec = (time_t) (totalTime / 1000000000LL);
	tp->tv_nsec = (long) (totalTime % 1000000000LL);
}

uint (*GET_TIME_OF_DAY)(struct timeval *tv, struct timezone *tz);
uint get_time_of_day(struct timeval *tv, struct timezone *tz) {
	//LOG_E("get_time_of_day");
	uint daytime;
	daytime = GET_TIME_OF_DAY(tv, tz);
	if (speed >= 1) {
		gettimeofday_hook_up(tv, tz);
	} else {
		gettimeofday_hook_down(tv, tz);
	}
	return daytime;
}

uint (*CLOCK_GETTIME)(clockid_t clk_id, struct timespec *tp);
uint clock_get_time(clockid_t clk_id, struct timespec *tp) {
	//LOG_E("clock_get_time");
	uint clocktime;
	if (clk_id != 1) {
		clocktime = CLOCK_GETTIME(clk_id, tp);
		if (speed >= 1) {
			clock_gettime_hook_up(tp);
		} else {
			clock_gettime_hook_down(tp);
		}
	} else {
		clocktime = CLOCK_GETTIME(clk_id, tp);
	}
	return clocktime;
}

#include <stdlib.h>
#include <string.h>
#include <errno.h>

/*
 * Avoid frequent malloc()/free() calls
 * (determined by getline() test on Linux)
 */
#define BUF_MIN 120

ssize_t getline(char **lineptr, size_t *n, FILE *stream)
{
    char *lptr;
    size_t len = 0;

    /* check for invalid arguments */
    if (lineptr == NULL || n == NULL) {
        errno = EINVAL;
        return -1;
    }

    lptr = fgetln(stream, &len);
    if (lptr == NULL) {
        /* invalid stream */
        errno = EINVAL;
        return -1;
    }

    /*
     * getline() returns a null byte ('\0') terminated C string,
     * but fgetln() returns characters without '\0' termination
     */
    if (*lineptr == NULL) {
        *n = BUF_MIN;
        goto alloc_buf;
    }

    /* realloc the original pointer */
    if (*n < len + 1) {
        free(*lineptr);

        *n = len + 1;
alloc_buf:
        *lineptr = (char*)malloc(*n);
        if (*lineptr == NULL) {
            *n = 0;
            return -1;
        }
    }

    /* copy over the string */
    memcpy(*lineptr, lptr, len);
    (*lineptr)[len] = '\0';

    /*
     * getline() and fgetln() both return len including the
     * delimiter but without the null byte at the end
     */
    return len;
}

long dllBaseAddr;
bool readmaps(pid_t target)
{
    FILE *maps;
    char name[128], *line = NULL;
    char exelink[128];
    size_t len = 0;
    unsigned int code_regions = 0, exe_regions = 0;
    unsigned long prev_end = 0, load_addr = 0, exe_load = 0;
    bool is_exe = false;

#define MAX_LINKBUF_SIZE 256
    char linkbuf[MAX_LINKBUF_SIZE], *exename = linkbuf;
    int linkbuf_size;
    char binname[MAX_LINKBUF_SIZE];

    /* check if target is valid */
    //if (target == 0)
    //    return false;

    /* construct the maps filename */
    //snprintf(name, sizeof(name), "/proc/%u/maps", target);
	snprintf(name, sizeof(name), "/proc/self/maps");

    /* attempt to open the maps file */
    if ((maps = fopen(name, "r")) == NULL) 
	{
        fprintf(stderr, "failed to open maps file %s.\n", name);
        return false;
    }

    fprintf(stderr, "maps file located at %s opened.\n", name);

    /* get executable name */
    //snprintf(exelink, sizeof(exelink), "/proc/%u/exe", target);
	snprintf(exelink, sizeof(exelink), "/proc/self/exe");
    linkbuf_size = readlink(exelink, exename, MAX_LINKBUF_SIZE - 1);
    if (linkbuf_size > 0)
    {
        exename[linkbuf_size] = 0;
    } else
	{
        /* readlink may fail for special processes, just treat as empty in
           order not to miss those regions */
        exename[0] = 0;
    }

    /* read every line of the maps file */
    while (getline(&line, &len, maps) != -1) 
	{
        unsigned long start, end;
        char read, write, exec, cow;
        int offset, dev_major, dev_minor, inode;

        /* slight overallocation */
        char filename[len];

        /* initialise to zero */
        memset(filename, '\0', len);

        /* parse each line */
        if (sscanf(line, "%lx-%lx %c%c%c%c %x %x:%x %u %[^\n]", &start, &end, &read,
                &write, &exec, &cow, &offset, &dev_major, &dev_minor, &inode, filename) >= 6) 
		{
			char* str = NULL;
			str = strstr(filename, "libpvz.so");
			//fprintf(stderr, "find %x ~ %x; %s\n", start, end, filename);
			if (str)
			{
				 fprintf(stderr, "---find %x ~ %x; %s\n", start, end, filename);
				 dllBaseAddr = start;
				 //dllBaseAddr = (void *)(start + 0x70A5F0 +1);
				 return true;
			}
           
        }
    }

    /* release memory allocated */
    free(line);
    fclose(maps);

    return true;
}

uint (*f__gettimeofday)(struct timeval *tv, struct timezone *tz);
uint (*f__clock_gettime)(clockid_t clk_id, struct timespec *tp);
void * resolve(void * agr) {
	void *handle = dlopen("/system/lib/libc.so", 1);
	void * symble = NULL;
	void * symbleClock = NULL;
	if (handle != NULL) {
		LOG_E("noEngine c exit");


		/*xhook_register("/system/lib/libc.so","gettimeofday",(void *) &get_time_of_day,(void**) &GET_TIME_OF_DAY);
		xhook_refresh(0);
        xhook_register("/system/lib/libc.so","clock_gettime",(void *) &clock_get_time,(void**) &CLOCK_GETTIME);
        xhook_refresh(0);*/
		
		//xhook修改的是其他so的对libc的引用，而不是libc本身
		/*int nRet1 = xhook_register(".*\.so$","gettimeofday",(void *) &get_time_of_day,(void**) &GET_TIME_OF_DAY);
        int nRet2 = xhook_register(".*\.so$","clock_gettime",(void *) &clock_get_time,(void**) &CLOCK_GETTIME);
        int nRet3 = xhook_refresh(0);
		LOG_E("%d - %d - %d", nRet1, nRet2, nRet3);*/
		
		readmaps(100);
		LOG_E("libpvz.so base addr: %x", dllBaseAddr);
		
		void* dll = NULL;
		dll = dlopen("libpvz.so", RTLD_NOW);
		char *error;
		f__gettimeofday = (uint(*)(struct timeval *, struct timezone *))dlsym(dll, "gettimeofday");
		if (f__gettimeofday)
			LOG_E("find gettimeofday.\n");
		else
			LOG_E("can not find gettimeofday.\n");
		
		if ((error = (char*)dlerror()) != NULL)  
			LOG_E("dlsym gettimeofday:	%s\n", error);
		
		Dl_info info;
		dladdr((void*)f__gettimeofday, &info);
		LOG_E("f__gettimeofday Dl_info: %x - %x\n", (unsigned int)(long)(info.dli_fbase), (unsigned int)(long)(info.dli_saddr));
	
		long fungettimeofdayAddr = (long)(info.dli_saddr);
		//long fungettimeofdayAddr = dllBaseAddr + 0x00051AD0;//gettimeofday	.plt	00051AD0	0000000C
		//LOG_E("%x - %x", fungettimeofdayAddr, (long)(info.dli_saddr));
		if (registerInlineHook((uint32_t) fungettimeofdayAddr, (uint32_t) (get_time_of_day), (uint32_t **) &GET_TIME_OF_DAY) != ELE7EN_OK) 
		{
			LOG_E("registerInlineHook gettimeofday failure\n");
		}
		if (inlineHook((uint32_t) fungettimeofdayAddr) != ELE7EN_OK) {
			LOG_E("inlineHook gettimeofday failure\n");
		}
		LOG_E("hook gettimeofday sucess\n");
		
		
		f__clock_gettime = (uint(*)(clockid_t , struct timespec *))dlsym(dll, "clock_gettime");
		if (f__clock_gettime)
			LOG_E("find clock_gettime.\n");
		else
			LOG_E("can not find clock_gettime.\n");
		
		char *error2;
		if ((error2 = (char*)dlerror()) != NULL)  
			LOG_E("dlsym clock_gettime:	%s\n", error2);
		
		Dl_info info2;
		dladdr((void*)f__clock_gettime, &info2);
		LOG_E("f__clock_gettime Dl_info: %x - %x\n", (unsigned int)(long)(info2.dli_fbase), (unsigned int)(long)(info2.dli_saddr));
	
		long funclock_gettimeAddr = (long)(info2.dli_saddr);
		//long funclock_gettimeAddr = dllBaseAddr + 0x00051C2C;//0x51C2C		clock_gettime	
		//LOG_E("%x - %x", funclock_gettimeAddr, (long)(info2.dli_saddr));
		if (registerInlineHook((uint32_t) funclock_gettimeAddr, (uint32_t) (clock_get_time), (uint32_t **) &CLOCK_GETTIME) != ELE7EN_OK) 
		{
			LOG_E("registerInlineHook clock_gettime failure\n");
		}
		if (inlineHook((uint32_t) funclock_gettimeAddr) != ELE7EN_OK) {
			LOG_E("inlineHook clock_gettime failure\n");
		}
		LOG_E("hook clock_gettime sucess\n");
	} else {
		LOG_E("noEngine c no exit");
		void * handler_ = dlopen("/system/lib/libc++.so", 1);
		if (handler_ != NULL) {
			LOG_E("noEngine c++ exit");
			xhook_register("/system/lib/libc++.so","gettimeofday",(void *) &get_time_of_day,(void**) &GET_TIME_OF_DAY);
			xhook_refresh(0);
            xhook_register("/system/lib/libc++.so","clock_gettime",(void *) &clock_get_time,(void**) &CLOCK_GETTIME);
            xhook_refresh(0);
			
			
		} else {
			LOG_E("noEngine c++ no exit");
		}
	}

	return (void *) 1;
}

void noEngine() {

	pthread_t pt;
	int thread;
	thread = pthread_create(&pt, NULL, (void *(*)(void *))resolve, NULL);
}

static const char Base64[] =
     "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
 static const char Pad64 = '=';

 int base64DeCode(char const *src, char *target, size_t targsize){
	size_t tarindex;
	int state, ch;
	char *pos;
	assert(src != NULL);
	assert(target != NULL);
	state = 0;
	tarindex = 0;
	while ((ch = (unsigned char) *src++) != '\0') {
		if (isspace(ch))
			continue;
		if (ch == Pad64)
			break;
		pos = (char *)strchr(Base64, ch);
		if (pos == 0)
			return (-1);
		switch (state) {
		case 0:
			if (target) {
				if (tarindex >= targsize)
					return (-1);
				target[tarindex] = (pos - Base64) << 2;
			}
			state = 1;
			break;
		case 1:
			if (target) {
				if (tarindex + 1 >= targsize)
					return (-1);
				target[tarindex] |=
					(u_int32_t)(pos - Base64) >> 4;
				target[tarindex+1]  = ((pos - Base64) & 0x0f)
							<< 4 ;
			}
			tarindex++;
			state = 2;
			break;
		case 2:
			if (target) {
				if (tarindex + 1 >= targsize)
					return (-1);
				target[tarindex] |=
					(u_int32_t)(pos - Base64) >> 2;
				target[tarindex+1] = ((pos - Base64) & 0x03)
							<< 6;
			}
			tarindex++;
			state = 3;
			break;
		case 3:
			if (target) {
				if (tarindex >= targsize)
					return (-1);
				target[tarindex] |= (pos - Base64);
			}
			tarindex++;
			state = 0;
			break;
		default:
			abort();
		}
	}
	if (ch == Pad64) {
		ch = *src++;
		switch (state) {
		case 0:
		case 1:
			return (-1);

		case 2:
			for (; ch != '\0'; ch = (unsigned char) *src++)
				if (!isspace(ch))
					break;
			if (ch != Pad64)
				return (-1);
			ch = *src++;
		case 3:
			for (; ch != '\0'; ch = (unsigned char) *src++)
				if (!isspace(ch))
					return (-1);
			if (target && target[tarindex] != 0)
				return (-1);
		}
	} else {
		if (state != 0)
			return (-1);
	}
	return (tarindex);
 }


typedef char *(*LUA_PCALL)(void *lua_state, int nargs, int nresults, int errfunc);
LUA_PCALL luapcall;

int  (* LUA_BUFFER)(void * lua_state,const char * buffer,long size,const char *name, const char *mode);

 int lua_buffer_hook(void * lua_state,const char * buffer,long size,const char *name, const char *mode){
	if(strstr(name,"ActivityHandler.lua")){
		char targetOne[336] = "";
		base64DeCode("LS1DQ0x1YUxvZygiISEg5pi+56S66Lez6L+HLeWktCIpCmxvY2FsIEJhdHRsZUxheWVyQ2xhc3MgPSByZXF1aXJlICJhcHAuc2NlbmVzLmJhdHRsZS5CYXR0bGVMYXllciIKbG9jYWwgY3Rvck9sZCA9IEJhdHRsZUxheWVyQ2xhc3MuY3RvcgpmdW5jdGlvbiBjdG9yTmV3KHNlbGYsIHBhY2ssIC4uLikKCS0tQ0NMdWFMb2coJyEhIHBhY2suc2tpcCA9ICcuLnRvc3RyaW5nKHBhY2suc2tpcCkpOwoJcGFjay5za2lwID0gMQoJbG9jYWwgcmV0ID0gY3Rvck9sZChzZWxmLCBwYWNrLCAuLi4pCglyZXR1cm4gcmV0CmVuZApCYXR0bGVMYXllckNsYXNzLmN0b3IgPSBjdG9yTmV3CgotLUNDTHVhTG9nKCIhISDmmL7npLrot7Pov4ct5bC+Iik=",targetOne,336);
		LUA_BUFFER(lua_state,targetOne,strlen(targetOne),"BattleSkipPatch.lua",mode);
		luapcall(lua_state,0,0,0);
//	}

//	if(strstr(name,"BattleSkipPatch.lua")){
		char targetTwo[608] = "";
		base64DeCode("Q0NMdWFMb2coIklOSkVDVCAhISEhISEhISEhISEhISEhISFHdWlkZU1hbmFnZXIgU3RhcnQiKQoKbG9jYWwgR3VpZGVNYW5hZ2VyQ2xhc3MgPSByZXF1aXJlICJhcHAuc2NlbmVzLmd1aWRlLkd1aWRlTWFuYWdlciIKCmxvY2FsIF9ydW5OZXh0R3VpZGVTdGVwID0gR3VpZGVNYW5hZ2VyQ2xhc3MuX3J1bk5leHRHdWlkZVN0ZXAKCmZ1bmN0aW9uIF9ydW5OZXh0R3VpZGVTdGVwTmV3KCBzZWxmLCAuLi4gKQoJQ0NMdWFMb2coIklOSkVDVCAhISEhISEhISEhISEhISEhISFHdWlkZU1hbmFnZXIuX3J1bk5leHRHdWlkZVN0ZXAiKQoJQ0NMdWFMb2coIiEhISEhISEhISEhISEhISEhIW1heDoiLi5zZWxmLl9tYXhHdWlkZVN0ZXApCglDQ0x1YUxvZygiISEhISEhISEhISEhISEhISEhY3VycmVudDoiLi5zZWxmLl9jdXJyZW50U3RlcCkKCglzZWxmLl9jdXJyZW50U3RlcCA9IDEwMDAwMDAxCgoJcmV0dXJuIF9ydW5OZXh0R3VpZGVTdGVwKHNlbGYsIC4uLikKZW5kCgpHdWlkZU1hbmFnZXJDbGFzcy5fcnVuTmV4dEd1aWRlU3RlcCA9IF9ydW5OZXh0R3VpZGVTdGVwTmV3",targetTwo,608);
		LUA_BUFFER(lua_state,targetTwo,strlen(targetTwo),"guideManagerV2.lua",mode);
		luapcall(lua_state,0,0,0);
	}

	 LOG_E("name = %s  \n  buffer = %s",name,buffer);




	return LUA_BUFFER(lua_state,buffer,size,name,mode);
 }

void luaBuffer(void * symble,const char *pathname_regex_str, const char *symbol){
	LOG_E("catchLuaBuffer");
	xhook_register(pathname_regex_str,symbol,(void *) &lua_buffer_hook,(void**) &LUA_BUFFER);
	xhook_refresh(0);
}




	//去除尾部空格
char *rtrim(char *str) {
	if (str == NULL || *str == '\0') {
		return str;
	}

	int len = strlen(str);
	char *p = str + len - 1;
	while (p >= str && isspace(*p)) {
		*p = '\0';
		--p;
	}

	return str;
}

int is_end_with(const char *str1, char *str2)
{
    if(str1 == NULL || str2 == NULL)
        return -1;
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    if((len1 < len2) ||  (len1 == 0 || len2 == 0))
        return -1;
    while(len2 >= 1)
    {
        if(str2[len2 - 1] != str1[len1 - 1])
            return 0;
        len2--;
        len1--;
    }
    return 1;
}

bool getLibHandler(char* path[1024], int count, bool runDefault) {

	bool hasSign = false;
	for (int k = 0; k < count; k++) {
		if (path[k]) {

//			char * r = (char *) malloc(1024 * sizeof(char));
//			strcpy(r, "/data/data/");
//			char append[20] = "/lib/";
//			strcat(r, pakcage);
//			strcat(r, append);
//			strcat(r, path[k]);
			void *handle_r = NULL;

			handle_r = dlopen(rtrim(path[k]), 1);
			if (handle_r != NULL) {
				void *lua = dlsym(handle_r,"luaL_loadbufferx");

				if(lua != NULL){
					luapcall = (LUA_PCALL)dlsym(handle_r,"lua_pcall");
					if(luapcall != NULL){
						luaBuffer(lua,rtrim(path[k]),"lua_pcall");
					}

				}

				if ((NULL != dlsym(handle_r, cocos2d_mornal[0]))|| (NULL != dlsym(handle_r, cocos2d_mornal[1]))) {
					hasSign = true;
					cocosNormal(handle_r,rtrim(path[k]));
				}
				if ((NULL != dlsym(handle_r, cocos2d_special[0]))|| (NULL != dlsym(handle_r, cocos2d_special[1])) || (NULL != dlsym(handle_r, cocos2d_special[2]))|| (NULL != dlsym(handle_r, cocos2d_special[3]))) {
					hasSign = true;
					cocosSpecail(handle_r,rtrim(path[k]));
				} else if (dlsym(handle_r, u3d_mono[0]) != NULL
						&& strstr(path[k], "mono")) {
					hasSign = true;
					u3dMono(handle_r,rtrim(path[k]));
				} else if (dlsym(handle_r, il2cpp_script[0])
						&& strstr(path[k], "il2cpp")) {
					hasSign = true;
					il2cppScript(handle_r,rtrim(path[k]));
				} else if (dlsym(handle_r, il2cpp_code[0])
						&& strstr(path[k], "il2cpp")) {
					hasSign = true;
					il2cppCode(handle_r,rtrim(path[k]));
				}



			} else {
				LOG_E("%s不存在!", path[k]);
			}
		}
	}
	if (!hasSign &&runDefault) {
		noEngine();
	}
	return hasSign;

}


char* * getDirectLibs(char * direct, const char * package){
	char * soes[1024] = { };
		int i = 0;
		DIR *dir;
		struct dirent *ptr;

		char * r = (char *) malloc(1024 * sizeof(char));
		strcpy(r, "/data/data/");
//		char append[20] = "/lib/";
		strcat(r, package);
		strcat(r, direct);

		dir = opendir(r);

		if (dir != NULL) {
			while ((ptr = readdir(dir)) != NULL) {
				LOG_E("位置 %s", ptr->d_name);
				if (strstr(ptr->d_name, ".so")
						&& 0 != strcmp(ptr->d_name, "libsubstrate.so")
						&& 0 != strcmp(ptr->d_name, "libGeeSingService.so")&& 0!= strcmp(ptr->d_name,"libunity.so")) {
					soes[i] = (char *) malloc(1024 * sizeof(char));
					strcpy(soes[i], ptr->d_name);
					i++;
				}
			}

			closedir(dir);

			for (int k = 0; k < i; k++) {
				if (soes[k]) {
					LOG_E("%s结果 %s",direct, soes[k]);
				}
			}
		} else {
			LOG_E("%s文件内容为空",direct);
		}
	return soes;
}

int is_begin_with(const char * str1,char *str2)
{
    if(str1 == NULL || str2 == NULL)
        return -1;
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    if((len1 < len2) ||  (len1 == 0 || len2 == 0))
        return -1;
    char *p = str2;
    int i = 0;
    while(*p != '\0')
    {
        if(*p != str1[i])
            return 0;
        p++;
        i++;
    }
    return 1;
}

/**判断str1是否以str2结尾
 * 如果是返回1
 * 不是返回0
 * 出错返回-1
 * */


int readFileList(char *basePath, char * charRecord [1024]){
      DIR *dir;
      struct dirent *ptr;
     char base[1000];

     if ((dir=opendir(basePath)) == NULL)
     {
    	 LOG_E("%s Open dir error...",basePath);
     }

     while ((ptr=readdir(dir)) != NULL)
     {
         if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)    ///current dir OR parrent dir
             continue;
         else if(ptr->d_type == 8){    ///file

        	 if (strstr(ptr->d_name, ".so")
        	      && 0 == strstr(ptr->d_name, "libsubstrate.so")  && 0 == strstr(ptr->d_name, "libunity.so")
        	        && 0 == strstr(ptr->d_name, "libGeeSingService.so") && is_begin_with("lib",ptr->d_name)
        	        && is_end_with(".so",ptr->d_name)) {

        	        	 charRecord[symIndex] = (char *) malloc(1024 * sizeof(char));
        	        	 char * directResult = (char *) malloc(1000 * sizeof(char));
        	        	 strcat(directResult,basePath);
        	        	 strcat(directResult,"/");
        	        	 strcat(directResult,ptr->d_name);

        	        	 LOG_E("文件名 = %s",ptr->d_name);
        	        	 strcpy(charRecord[symIndex], directResult);

        	        	 symIndex++;
        	 }
         }



//         else if(ptr->d_type == 10)    ///link file
//        	 LOG_E("d_name2:%s/%s\n",basePath,ptr->d_name);
         else if(ptr->d_type == 4 || ptr->d_type == 10)    ///dir
         {
             memset(base,'\0',sizeof(base));
             strcpy(base,basePath);
             strcat(base,"/");
             strcat(base,ptr->d_name);
             readFileList(base,charRecord);
         }
     }
     closedir(dir);
     return 1;
 }


void * recheck(void * agr) {
	reckeckCount = 0;
	while(1){
		if(reckeckCount < 60){
			sleep(3);
			char * soes[1024] = { };
			symIndex = 0;
			readFileList(passBaseDirect,soes);

			//读取附加文件夹  有的引擎不放在lib里面
			for (int k = 0; k < symIndex; k++) {
				if (soes[k]) {
					LOG_E("结果 %s",soes[k]);
				}
			}

			if(getLibHandler(soes, symIndex,false)){
				break;
			}
		}else{
			reckeckCount++;
		}
	}




	return (void *) 1;
}


void (*MY_EXIT)(int status);




int (*MY_PTHREAD_KILL)(pthread_t threadid, int signo);

int my_pthread_kill(pthread_t threadid, int signo){

	LOG_E("my_pthread_kill");



	return MY_PTHREAD_KILL(threadid,0);
}


//char * (* FILE_FGETS)(char * s, int n,FILE *stream);
//
//char * file_fgets(char * s, int n,FILE *stream){
//
//	char * tem = FILE_FGETS(s,n,stream);
//
////	LOG_E("读取 = %s",tem);
//	if(strstr(tem,"libsubstrate.so")){
//		LOG_E("原本 = %s",tem);
//		tem = FILE_FGETS(s,n,stream);
//		LOG_E("下一句 = %s",tem);
//	}
//
//	return tem;
//}

void Java_com_longshao_aiquyouxi_AiQuYouXi_AiQuYouXiAddTime(JNIEnv *env,
		jclass ywcls) {
	noEngine();
}



jint Java_com_longshao_aiquyouxi_AiQuYouXi_AiQuYouXiui(JNIEnv *env,
		jclass ywcls) {

	speed = 1.0;
	jstring p = get_p(env, ywcls);
	jstring si = get_s(env, ywcls, p);
	char *c_si = (char*) env->GetStringUTFChars(si, 0);

	//筛查so
	const char * package = (char*) env->GetStringUTFChars(p, 0);

	//防止部分so文件不放在lib路径，遍历/data/data/pkg文件夹

	char * baseDirect = (char *) malloc(1024 * sizeof(char));
	strcpy(baseDirect, "/data/data/");
	strcat(baseDirect, package);

	passBaseDirect = baseDirect;

	char * soes[1024] = { };
	symIndex = 0;
	readFileList(baseDirect,soes);//预防引擎包不放在lib里面,直接遍历内存储

//	checkAiQufile(c_si,env);

//	checkfile(c_si,env);

	//读取data/data/pkg/lib文件夹

	//读取附加文件夹  有的引擎不放在lib里面
	for (int k = 0; k < symIndex; k++) {
		if (soes[k]) {
			LOG_E("结果 %s",soes[k]);
		}
	}

	bool hasSign = getLibHandler(soes, symIndex,true);

	if(!hasSign){
		pthread_t pthreadsign;
		int signthread;
		signthread = pthread_create(&pthreadsign, NULL, (void *(*)(void *))recheck, NULL);
	}

	return 0;
}

//---------------------------------------------------------------------------------
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>

#include <string.h>

char g_szDefaultSocketName[64] = "cheat";
int g_socket = 0;
int g_port = 1166;
bool g_cheatSDKFinish = false;

int start_server(char* sockname)
{
	int fd = socket(AF_INET, SOCK_STREAM, 0);

	if (fd < 0)
	{
		LOG_E("creating socket");
		return fd;
	}

	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(g_port);

	int opt;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (void*)&opt, sizeof(opt));

	if (bind(fd, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0)
	{
		LOG_E("binding socket");
		close(fd);
		return -1;
	}

	listen(fd, 1);

	return fd;
}

void process_cmd(char* io_buffer)
{
	LOG_E("process_cmd io_buffer:	%s\n", io_buffer);
	int index;
	char* nextPtr;
	void* addr;
	int data;
	char* cursor = io_buffer;
	cursor += 1;
	int status;
	switch (io_buffer[0])
	{
	case 'D':
		{
			char strCheatCmd[128];
			memset(strCheatCmd, '0', 128);
			sscanf(io_buffer, "D: %s", strCheatCmd);
			//CCheatSDK::getInstance()->setCallFunFlag(strCheatCmd);
			LOG_E("speed 8\n");
			speed = 8.0;
		}
		break;
	case 'Z':
		{
			g_cheatSDKFinish = true;
			//CCheatSDK::getInstance()->cheatSDKFinish();
		}
		break;
	default:
		break;
	}
}


static pthread_t cheat_thread;
void* threadProc(void* param)
{
	char* sockname = g_szDefaultSocketName;
	struct sockaddr_un client_addr;
	socklen_t client_addr_length = sizeof(client_addr);

	int server_fd = start_server(sockname);
	if (server_fd < 0)
	{
		LOG_E("start server failed:Unable to start server on %s\n", sockname);
		LOG_E("maybe have already start a server.\n");
		return NULL;
	}
	LOG_E( "start server succeeded\n");
	while (1)
	{
		if (g_cheatSDKFinish)
			break;
		int client_fd = accept(server_fd, (struct sockaddr*) NULL, NULL);

		if (client_fd < 0)
		{
			LOG_E("accepting client exit(1)\n");
			//exit(1);
			continue;
		}

		LOG_E("Connection established\n");

		char io_buffer[80] = { 0 };
		int io_length = 0;
		char* cursor;
		long int contact, x, y, pressure;

		while (1) {
			io_length = 0;

			while (io_length < sizeof(io_buffer)
					&& recv(client_fd, &io_buffer[io_length], 1, 0) == 1) {
				if (io_buffer[io_length++] == '\n') {
					break;
				}
			}

			if (io_length <= 0) {
				LOG_E("judgement: client %d have exited.\n", client_fd);
				break;
			}

			if (io_buffer[io_length - 1] != '\n') {
				continue;
			}

			if (io_length == 1) {
				continue;
			}

			process_cmd(io_buffer);
		}

		LOG_E("client_fd %d Connection closed\n", client_fd);
		close(client_fd);
	}

	close(server_fd);
	LOG_E("exit cheat thread\n");
    return NULL;
}

typedef union {
	JNIEnv* env;
	void* venv;
} UnionJNIEnvToVoid;

__attribute__((visibility("default")))
jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	UnionJNIEnvToVoid uenv;
	uenv.venv = NULL;
	jint result = -1;
	JNIEnv* env = NULL;

	LOG_E("JNI_OnLoad.\n");

	if (vm->GetEnv(&uenv.venv, JNI_VERSION_1_4) != JNI_OK) {
		LOG_E("ERROR: GetEnv failed.\n");
		return result;
	}
	env = uenv.env;
	
	pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_create(&cheat_thread, &attr, threadProc, NULL);

	noEngine();
	
	result = JNI_VERSION_1_4;
	return result;
}

int main(int argc, char** argv)
{
	if (getuid() != 0)
        fprintf(stderr, "*** RUNNING AS unROOT. ***\n");
	else
		fprintf(stderr, "*** RUNNING AS ROOT. ***\n");
	
	bool bForTest[2] = {false, false};
	struct sockaddr_in addr;
	g_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (g_socket < 0)
	{
		fprintf(stderr, "creating socket error\n");
		return 1;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET; //设置为IP通信
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(g_port);

	if (connect(g_socket, (struct sockaddr *) &addr, sizeof(addr)) < 0)
	{
		fprintf(stderr, "connect socket error %d\n", errno);
		close(g_socket);
		return 1;
	}
	fprintf(stderr, "connect socket sucess %d\n", g_socket);
		
	while (1)
	{
		fprintf(stderr, "input commonds>");
		char cmdBuf[128] = {};
		scanf("%s", cmdBuf);
		if (0 == strcmp(cmdBuf, "exit"))
		{
			fprintf(stderr, "exit cmd\n");
			break;
		}
		else if (0 == strcmp(cmdBuf, "help"))
		{
			;
		}
		else if (0 == strcmp(cmdBuf, "sdkFinish"))
		{
			char cmd[80] = { 0 };
			int io_length;
			io_length = snprintf(cmd, sizeof(cmd), "Z\n");
			send(g_socket, cmd, io_length, 0);
			
			fprintf(stderr, "exit cmd\n");
			break;
		}
		else
		{
			char cmd[80] = { 0 };
			int io_length;
			io_length = snprintf(cmd, sizeof(cmd), "D: speedup\n");
			send(g_socket, cmd, io_length, 0);
		}
	}
	
	
	fprintf(stderr, "socket close %d\n", g_socket);
	close(g_socket);
	return 0;
}
