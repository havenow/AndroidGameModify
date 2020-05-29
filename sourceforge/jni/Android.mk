LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_C_INCLUDES += bionic $(LOCAL_PATH)/include

LOCAL_MODULE := scanmem

LOCAL_SRC_FILES := commands.c \
list.c \
maps.c \
ptrace.c \
show_message.c \
value.c \
handlers.c \
main.c \
menu.c \
scanroutines.c \
target_memory_info_array.c \
lib/getline.c \
lib/getdelim.c

#error: only position independent executables (PIE) are supported.
# Enable PIE manually. Will get reset on $(CLEAR_VARS).
LOCAL_CFLAGS += -fPIE
LOCAL_LDFLAGS += -fPIE -pie

include $(BUILD_EXECUTABLE)
