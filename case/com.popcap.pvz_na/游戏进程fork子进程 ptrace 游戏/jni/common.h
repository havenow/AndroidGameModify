#ifndef _COMMON_H
#define _COMMON_H

#include <unwind.h>
#include <dlfcn.h>
#include <vector>
#include <string> 
#include <dirent.h> 
#include <stdlib.h>

#include <android/log.h>
#ifndef DEBUG
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,"inlinehook",__VA_ARGS__)
#else
#define LOGE(...)
#endif

void callstackLogcat(int prio, const char* tag);
void appendToLogFile(const char *format, ...);
int find_pid_of(const char *process_name);

#endif