LOCAL_PATH := $(call my-dir)

#--------------------------------------------------------
include $(CLEAR_VARS)
LOCAL_MODULE    := hook
LOCAL_SRC_FILES := inlinehook\inlineHook.c inlinehook\relocate.c
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := inlineHook
LOCAL_SRC_FILES :=  common.cpp \
					bleach_bvn2\main.cpp \
					bleach_bvn2\cheatBleachBvn.cpp \
					
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include
#LOCAL_STATIC_LIBRARIES := libhook
#LOCAL_CFLAGS := -funwind-tables
#LOCAL_CPPFLAGS := -fvisibility=hidden
LOCAL_LDLIBS := -llog 
include $(BUILD_SHARED_LIBRARY)

#--------------------------------------------------------
include $(CLEAR_VARS)
LOCAL_MODULE    := cheatJ
LOCAL_SRC_FILES :=  common.cpp \
					bleach_bvn2\main.cpp \
					bleach_bvn2\cheatBleachBvn.cpp \
					
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_STATIC_LIBRARIES := libhook
#LOCAL_CFLAGS := -funwind-tables
LOCAL_LDLIBS := -llog 

LOCAL_CFLAGS += -pie -fPIE
LOCAL_LDFLAGS += -pie -fPIE

include $(BUILD_EXECUTABLE)
