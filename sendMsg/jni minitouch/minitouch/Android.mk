LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := papastick-common

LOCAL_SRC_FILES := \
	minitouch.cpp \

LOCAL_STATIC_LIBRARIES := \
	libevdev \

include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)

# Enable PIE manually. Will get reset on $(CLEAR_VARS).
LOCAL_CFLAGS += -fPIE
LOCAL_LDFLAGS += -fPIE -pie

LOCAL_MODULE := papastick

LOCAL_STATIC_LIBRARIES := papastick-common

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_MODULE := papastick-nopie

LOCAL_STATIC_LIBRARIES := papastick-common

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_MODULE := papa_stick

LOCAL_STATIC_LIBRARIES := papastick-common

include $(BUILD_SHARED_LIBRARY)
