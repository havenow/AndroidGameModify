LOCAL_PATH := $(call my-dir)

###########################################################################	elf
include $(CLEAR_VARS)


LOCAL_MODULE := elfTest

LOCAL_C_INCLUDES := $(LOCAL_PATH)\

LOCAL_SRC_FILES := main.cpp \
					list.c \
					show_message.c \
					commands.c \
					handlers.c \
					menu.c \
					getdelim.c \
					getline.c \
					scanmem.c \
					maps.c \
					


# Enable PIE manually. Will get reset on $(CLEAR_VARS).
LOCAL_CFLAGS += -fPIE
LOCAL_LDFLAGS += -fPIE -pie

LOCAL_LDLIBS := -llog

include $(BUILD_EXECUTABLE)



