#
```
1、发布的时候日志不输出
#include <android/log.h>
#ifdef DEBUG
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,"inlinehook",__VA_ARGS__)
#else
#define LOGE(...)
#endif

2、编译时隐藏符号
Android.mk添加
LOCAL_CPPFLAGS := -fvisibility=hidden

```
