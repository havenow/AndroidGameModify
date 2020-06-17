LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := inject 
LOCAL_SRC_FILES := inject.c \
shellcode.s

#shellcode.s

LOCAL_LDLIBS += -L$(SYSROOT)/usr/lib -llog

LOCAL_CFLAGS += -pie -fPIE
LOCAL_LDFLAGS += -pie -fPIE

#LOCAL_FORCE_STATIC_EXECUTABLE := true

include $(BUILD_EXECUTABLE)


#################################
#LOCAL_PATH := $(call my-dir)
 
include $(CLEAR_VARS)
 
LOCAL_LDLIBS += -L$(SYSROOT)/usr/lib -llog 
#LOCAL_ARM_MODE := arm
LOCAL_MODULE    := hello
LOCAL_SRC_FILES := hello.c
include $(BUILD_SHARED_LIBRARY)
