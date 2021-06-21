LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := test-debug-ndk

LOCAL_SRC_FILES := main.cpp \

LOCAL_CPPFLAGS := -fvisibility=hidden

LOCAL_ARM_MODE := arm
LOCAL_CFLAGS += -fPIE
LOCAL_LDFLAGS += -fPIE -pie

include $(BUILD_EXECUTABLE)



