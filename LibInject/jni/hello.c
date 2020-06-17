#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <android/log.h>
#include <elf.h>
#include <fcntl.h>
 
#define LOG_TAG "DEBUG"
#define LOGD(fmt, args...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, fmt, ##args)  
 
int hook_entry(char * a){
    LOGD("Hook success, pid = %d\n", getpid());
    LOGD("Hello %s\n", a);
    return 0;
}
