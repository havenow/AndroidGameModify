LOCAL_PATH := $(call my-dir)

SRC_PATH := .
HEADER_PATH := $(LOCAL_PATH)


include $(CLEAR_VARS)
LOCAL_MODULE := crackWudi 
LOCAL_SRC_FILES := crack.c \

LOCAL_LDLIBS += -L$(SYSROOT)/usr/lib -llog

LOCAL_CFLAGS += -pie -fPIE
LOCAL_LDFLAGS += -pie -fPIE

#LOCAL_FORCE_STATIC_EXECUTABLE := true

include $(BUILD_EXECUTABLE)