LOCAL_PATH := $(call my-dir)



include $(CLEAR_VARS)

# Enable PIE manually. Will get reset on $(CLEAR_VARS).
LOCAL_CFLAGS += -fPIE
LOCAL_LDFLAGS += -fPIE -pie

LOCAL_MODULE := sendMsg

LOCAL_SRC_FILES := \
	minitouch.cpp \

include $(BUILD_EXECUTABLE)

