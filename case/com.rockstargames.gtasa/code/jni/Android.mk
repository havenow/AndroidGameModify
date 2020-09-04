LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := hook
LOCAL_SRC_FILES := inlineHook.c relocate.c
include $(BUILD_STATIC_LIBRARY)

#--------------------------------------------------------
include $(CLEAR_VARS)
LOCAL_MODULE    := inlineHook
LOCAL_SRC_FILES := cheatMain.cpp \
					cheatI.cpp \
					cheatGTASA.cpp \
					cheatGTAVC.cpp \
					cheatMgr.cpp \
					hookGames\hookGTASA.cpp \
					hookGames\hookGTAVC.cpp \
					common.cpp \
					
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_STATIC_LIBRARIES := libhook
#LOCAL_CFLAGS := -funwind-tables
LOCAL_LDLIBS := -llog 
include $(BUILD_SHARED_LIBRARY)

#--------------------------------------------------------
include $(CLEAR_VARS)
LOCAL_MODULE    := cheatJ
LOCAL_SRC_FILES :=  cheatMain.cpp \
					cheatI.cpp \
					cheatGTASA.cpp \
					cheatGTAVC.cpp \
					cheatMgr.cpp \
					hookGames\hookGTASA.cpp \
					hookGames\hookGTAVC.cpp \
					common.cpp \
					
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_STATIC_LIBRARIES := libhook
#LOCAL_CFLAGS := -funwind-tables
LOCAL_LDLIBS := -llog 

LOCAL_CFLAGS += -pie -fPIE
LOCAL_LDFLAGS += -pie -fPIE

include $(BUILD_EXECUTABLE)
