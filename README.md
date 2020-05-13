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

# 安卓通过socket给elf发送命令，修改内存
/data/local/tmp目录下面的elf可以运行（chmod 777），程序入口函数是main    
安卓上层调用so里面的本地方法     
socket都是用c++实现     

# strtoll打印时导致的crash
```c++
char* io_buffer = "W A8F79F30 1222";
int data;
char *ptr;
char *ptr1;

char* cursor = io_buffer;
cursor += 1;
void * addr = (void *)strtoll(cursor, &ptr, 16);
printf("%0x", addr);//打印 A8F79F30
//printf("%s", addr);会崩溃
int value = strtol(ptr, &ptr, 10);
```

# Android studio ndk samples
https://github.com/android/ndk-samples     
ndk-samples\other-builds\ndkbuild\two-libs      
这个例子是ndk还是使用android.mk来编译，但是c++代码是无法调试的，也看不了c++代码      
Android Studio只能写CMakeList.txt，才能调试c++代码？？？     
```
    externalNativeBuild {
        ndkBuild {
            path 'src/main/cpp/Android.mk'
        }
    }
```
使用夜神模拟器只能跑程序，无法调试程序???     



# ndk里面函数如果最后无返回值，Android Studio调试此函数时会崩溃
```
static jint init_native(JNIEnv *env, jobject thiz)
{
	... ...
	//return 0;
}
```

# Android Studio调试c++代码
真机可以进入java和c++断点					
安卓自带的模拟器可以进入java和c++断点					
夜神模拟器可以进入java断点，无法进入c++断点，没有找到原因				
(夜神模拟器应该需要使用x86的so，使用了还是无法进入c++断点)			
```
       ndk {
            abiFilters 'armeabi-v7a', 'arm64-v8a', 'x86', 'x86_64'
        }
```		

# getuid()
```
     root的手机  uid = 2000，用adb shell ps -ef查看，uid是shell
     夜神模拟    uid = 0
	 uid为0时ptrace(PTRACE_DETACH,才会成功
```
