LOCAL_PATH := $(call my-dir)
APP_ABI := armeabi-v7a
#第三方
include $(CLEAR_VARS)
LOCAL_MODULE    := xhook
LOCAL_SRC_FILES := armeabi-v7a/libxhook.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
include $(PREBUILT_SHARED_LIBRARY)

#第一方
include $(CLEAR_VARS)
LOCAL_MODULE    := GeeSingService
LOCAL_SRC_FILES := GeeSingService.cpp
LOCAL_LDLIBS    += -L$(SYSROOT)/lib -llog
LOCAL_CFLAGS    := -g
#第三方编译
LOCAL_SHARED_LIBRARIES := xhook
include $(BUILD_SHARED_LIBRARY)

#--------------------------------------------------------
include $(CLEAR_VARS)
LOCAL_MODULE    := speed
LOCAL_SRC_FILES :=  test.cpp \
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include
#LOCAL_CFLAGS := -funwind-tables
LOCAL_LDLIBS := -llog 

LOCAL_CFLAGS += -pie -fPIE
LOCAL_LDFLAGS += -pie -fPIE

include $(BUILD_EXECUTABLE)


