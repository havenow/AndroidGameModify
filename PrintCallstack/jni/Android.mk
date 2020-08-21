
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := callStack
LOCAL_SRC_FILES := printCallstackMain.cpp
#LOCAL_CFLAGS := -funwind-tables
LOCAL_LDLIBS := -llog 
include $(BUILD_SHARED_LIBRARY)
