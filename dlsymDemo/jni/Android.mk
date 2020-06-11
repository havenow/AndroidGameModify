LOCAL_PATH := $(call my-dir)


include $(CLEAR_VARS)

SRC_PATH := .
HEADER_PATH := $(LOCAL_PATH)

LOCAL_MODULE    := pgod
LOCAL_SRC_FILES := $(SRC_PATH)/god-bridge.cpp


LOCAL_LDLIBS := -llog

include $(BUILD_SHARED_LIBRARY)

######################

include $(CLEAR_VARS)

SRC_PATH := .
HEADER_PATH := $(LOCAL_PATH)

LOCAL_MODULE    := pgod-test
LOCAL_SRC_FILES := $(SRC_PATH)/god.cpp


LOCAL_LDLIBS := -llog

include $(BUILD_SHARED_LIBRARY)

