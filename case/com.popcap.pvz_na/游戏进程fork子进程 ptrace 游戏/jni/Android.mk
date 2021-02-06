LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := pgod
LOCAL_SRC_FILES := main.cpp \
					common.cpp
LOCAL_LDLIBS    += -L$(SYSROOT)/lib -llog
LOCAL_CFLAGS    := -g

LOCAL_CPPFLAGS := -fvisibility=hidden
ifeq ($(NDK_DEBUG), 1)
LOCAL_CPPFLAGS += -DDEBUG
endif
LOCAL_LDLIBS := -llog 
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


