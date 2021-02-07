#使用r16或者以上编译 libunwind.a
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := membreakpoint
LOCAL_SRC_FILES :=  main.cpp \
					
#LOCAL_STATIC_LIBRARIES := libunwind.a	
#LOCAL_CFLAGS := -funwind-tables
LOCAL_LDLIBS := -llog 
include $(BUILD_SHARED_LIBRARY)


#-----------------------------------------
include $(CLEAR_VARS)
LOCAL_MODULE    := memWatch
LOCAL_SRC_FILES := main.cpp 
				
LOCAL_LDLIBS    += -L$(SYSROOT)/lib -llog 
LOCAL_CFLAGS    := -g

#LOCAL_STATIC_LIBRARIES := libunwind.a
LOCAL_CPPFLAGS := -fvisibility=hidden -fpermissive
LOCAL_CFLAGS += -pie -fPIE
LOCAL_LDFLAGS += -pie -fPIE

include $(BUILD_EXECUTABLE)



