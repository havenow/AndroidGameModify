# APP_ABI := armeabi-v7a x86
armeabi-v7a 在手机上运行    
x86	在夜神模拟器上运行     

# so无法进入JNI_OnLoad
编译elf需要添加pie，编译so不需要    
```
LOCAL_CFLAGS += -fPIE     
LOCAL_LDFLAGS += -fPIE -pie    
```

# 两种调用方式     
在夜神模拟上面通过so方式ptrace(PTRACE_ATTACH会失败，没找到原因     
在夜神模拟上面通过eof方式ptrace(PTRACE_ATTACH成功     

# Android Studio 利用CMake生成的是ninja
ninja是一个小型的关注速度的构建系统，不需要关心ninja的脚本，知道怎么配置CMake就可以了      

