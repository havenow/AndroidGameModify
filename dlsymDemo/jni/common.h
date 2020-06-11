//
// Created by wangf on 2020/5/13.
//

#ifndef DEMO_COMMON_H
#define DEMO_COMMON_H


#include <jni.h>
#include <android/log.h>

#define LOG_PRINT
#ifdef LOG_PRINT

#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,"scanmem",__VA_ARGS__)
#define ELF_LOG(...) fprintf (stderr, __VA_ARGS__)
#define ANDROID_ELF_LOG(...) __android_log_print(ANDROID_LOG_ERROR,"scanmem",__VA_ARGS__); fprintf (stderr, __VA_ARGS__)

#else

#define  LOGE(...)
#define ELF_LOG(...)
#define ANDROID_ELF_LOG(...)
#endif

#endif //DEMO_COMMON_H
