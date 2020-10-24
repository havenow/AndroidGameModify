# 
```
adb push ./libtest.so ./main /data/local/tmp
adb shell "chmod +x /data/local/tmp/main"
adb shell "export LD_LIBRARY_PATH=/data/local/tmp"

```

#
```
include $(CLEAR_VARS)
LOCAL_MODULE            := test
LOCAL_SRC_FILES         := $(LOCAL_PATH)/../../../libtest/libs/$(TARGET_ARCH_ABI)/libtest.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../../libtest/jni
include $(PREBUILT_SHARED_LIBRARY)
```