LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := pgod
LOCAL_SRC_FILES := main.cpp \
					common.cpp
LOCAL_LDLIBS    += -L$(SYSROOT)/lib -llog
LOCAL_CFLAGS    := -g

LOCAL_CPPFLAGS := -fvisibility=hidden -fpermissive
ifeq ($(NDK_DEBUG), 1)
LOCAL_CPPFLAGS += -DDEBUG
endif
LOCAL_LDLIBS := -llog 
include $(BUILD_SHARED_LIBRARY)



