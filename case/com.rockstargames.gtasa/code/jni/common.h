#ifndef _COMMON_H
#define _COMMON_H

#include <dlfcn.h>

#include <android/log.h>
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,"inlinehook",__VA_ARGS__)

#endif