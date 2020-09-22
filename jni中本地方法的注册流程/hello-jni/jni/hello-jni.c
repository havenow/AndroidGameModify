/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <string.h>
#include <jni.h>
#include <android/log.h>
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,"testDlsym",__VA_ARGS__)

/* This is a trivial JNI example where we use a native method
 * to return a new VM String. See the corresponding Java source
 * file located at:
 *
 *   apps/samples/hello-jni/project/src/com/example/hellojni/HelloJni.java
 */
jstring
Java_com_dsemu_drastic_DraSticActivity_stringFromJNI( JNIEnv* env,
                                                  jobject thiz )
{
	LOGE("Java_com_dsemu_drastic_DraSticActivity_stringFromJNI called");
    return (*env)->NewStringUTF(env, "Hello from JNI !");
}

JNIEXPORT jstring JNICALL native_hello(JNIEnv *env, jclass clazz)
{
	LOGE("native_hello called");
        printf("hello in c native code./n");
        return (*env)->NewStringUTF(env, "hello world returned.");
}

#define JNIREG_CLASS "com/dsemu/drastic/DraSticActivity"

static JNINativeMethod gMethods[] = {
        { "hello", "()Ljava/lang/String;", (void*)native_hello },
};

static int registerNativeMethods(JNIEnv* env, const char* className,
        JNINativeMethod* gMethods, int numMethods)
{
	LOGE("111111111111");
        jclass clazz;
        clazz = (*env)->FindClass(env, className);
        LOGE("22222222222");
        if (clazz == NULL) {
                return JNI_FALSE;
        }
        LOGE("33333333333");
        if ((*env)->RegisterNatives(env, clazz, gMethods, numMethods) < 0) {
                return JNI_FALSE;
        }
        LOGE("44444444");
        return JNI_TRUE;
}


static int registerNatives(JNIEnv* env)
{
        if (!registerNativeMethods(env, JNIREG_CLASS, gMethods,

                                 sizeof(gMethods) / sizeof(gMethods[0])))
                return JNI_FALSE;

        return JNI_TRUE;
}

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	JNIEnv* env = NULL;
	if ((*vm)->GetEnv(vm, (void**)&env, JNI_VERSION_1_4) != JNI_OK)
	{
		return -1;
	}

	registerNatives(env);

	return JNI_VERSION_1_4;

}

