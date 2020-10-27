# 
```
adb push ./libtest.so ./main /data/local/tmp
adb shell "chmod +x /data/local/tmp/main"
adb shell "export LD_LIBRARY_PATH=/data/local/tmp"

```

#
```
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE            := test
LOCAL_SRC_FILES         := $(LOCAL_PATH)/../../../libtest/libs/$(TARGET_ARCH_ABI)/libtest.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../../libtest/jni
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE            := main
LOCAL_SRC_FILES         := main.c
LOCAL_SHARED_LIBRARIES  := test
LOCAL_CFLAGS            := -Wall -Wextra -fPIE
LOCAL_CONLYFLAGS        := -std=c11
LOCAL_LDLIBS            += -fPIE -pie
include $(BUILD_EXECUTABLE)
```

# 编译
```
libxhook下面的工程编译需要用  Android NDK r16b
如果使用 Android NDK r10e，会报错
[armeabi-v7a] Compile thumb  : xhook <= xh_elf.c
jni/xh_elf.c: In function 'xh_elf_init':
jni/xh_elf.c:848:14: error: 'DT_ANDROID_REL' undeclared (first use in this function)
         case DT_ANDROID_REL:
              ^
```

# API
```
libxhook/jni/xhook.h

1. Register hook info
int xhook_register(const char  *pathname_regex_str,  
                   const char  *symbol,  
                   void        *new_func,  
                   void       **old_func);
				   
Return:					zero if successful, non-zero otherwise.
pathname_regex_str:		In current process's memory space, in every loaded ELF which pathname matches regular expression pathname_regex_str, every PLT entries to symbol will be replaced with new_func. The original one will be saved in old_func.
The new_func must have the same function declaration as the original one.
The regular expression for pathname_regex_str only support POSIX BRE (Basic Regular Expression).
```
