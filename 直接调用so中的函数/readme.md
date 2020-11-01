# 
```
adb push ./libtest.so ./main /data/local/tmp
adb shell "chmod +x /data/local/tmp/main"
adb shell "export LD_LIBRARY_PATH=/data/local/tmp"

```

#
```
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE            := test
LOCAL_SRC_FILES         := $(LOCAL_PATH)/../../../libtest/libs/$(TARGET_ARCH_ABI)/libtest.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../../libtest/jni
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE            := main
LOCAL_SRC_FILES         := main.c
LOCAL_SHARED_LIBRARIES  := test
LOCAL_CFLAGS            := -Wall -Wextra -fPIE
LOCAL_CONLYFLAGS        := -std=c11
LOCAL_LDLIBS            += -fPIE -pie
include $(BUILD_EXECUTABLE)
```

# 编译
```
libxhook下面的工程编译需要用  Android NDK r16b
如果使用 Android NDK r10e，会报错
[armeabi-v7a] Compile thumb  : xhook <= xh_elf.c
jni/xh_elf.c: In function 'xh_elf_init':
jni/xh_elf.c:848:14: error: 'DT_ANDROID_REL' undeclared (first use in this function)
         case DT_ANDROID_REL:
              ^
```

# API
```
libxhook/jni/xhook.h

1. Register hook info
int xhook_register(const char* pathname_regex_str, const char* symbol, void* new_func, void** old_func);
				   
Return:			zero if successful, non-zero otherwise.
args:			In current process's memory space, in every loaded ELF which pathname matches regular expression pathname_regex_str, every PLT entries to symbol will be replaced with new_func. The original one will be saved in old_func.
The new_func must have the same function declaration as the original one.
The regular expression for pathname_regex_str only support POSIX BRE (Basic Regular Expression).

注册 hook 信息
在当前进程的内存空间中，在每一个符合正则表达式 pathname_regex_str 的已加载ELF中，每一个调用 symbol 的 PLT 入口点的地址值都将给替换成 new_func。之前的 PLT 入口点的地址值将被保存在 old_func 中。
new_func 必须具有和原函数同样的函数声明。
成功返回 0，失败返回 非0。
pathname_regex_str 只支持 POSIX BRE (Basic Regular Expression) 定义的正则表达式语法。

2. Ignore some hook info
int xhook_ignore(const char *pathname_regex_str, const char *symbol);
				 
Return:			zero if successful, non-zero otherwise.
args:			Ignore some hook info according to pathname_regex_str and symbol, from registered hooks by xhook_register. If symbol is NULL, xhook will ignore all symbols from ELF which pathname matches pathname_regex_str.
The regular expression for pathname_regex_str only support POSIX BRE.

忽略部分 hook 信息
根据 pathname_regex_str 和 symbol，从已经通过 xhook_register 注册的 hook 信息中，忽略一部分 hook 信息。如果 symbol 为 NULL，xhook 将忽略所有路径名符合正则表达式 pathname_regex_str 的 ELF。
成功返回 0，失败返回 非0。
pathname_regex_str 只支持 POSIX BRE 定义的正则表达式语法。

3. Do hook
int xhook_refresh(int async);

Return:			zero if successful, non-zero otherwise.
args:			Do the real hook operations according to the registered hook info.
Pass 1 to async for asynchronous hook. Pass 0 to async for synchronous hook.
xhook will keep a global cache for saving the last ELF loading info from /proc/self/maps. This cache will also be updated in xhook_refresh. With this cache, xhook_refresh can determine which ELF is newly loaded. We only need to do hook in these newly loaded ELF.

执行 hook
根据前面注册的 hook 信息，执行真正的 hook 操作。
给 async 参数传 1 表示执行异步的 hook 操作，传 0 表示执行同步的 hook 操作。
成功返回 0，失败返回 非0。
xhook 在内部维护了一个全局的缓存，用于保存最后一次从 /proc/self/maps 读取到的 ELF 加载信息。每次一调用 xhook_refresh 函数，这个缓存都将被更新。xhook 使用这个缓存来判断哪些 ELF 是这次新被加载到内存中的。我们每次只需要针对这些新加载的 ELF 做 hook 就可以了。

4. Clear cache
void xhook_clear();
Clear all cache owned by xhook, reset all global flags to default value.
If you confirm that all PLT entries you want have been hooked, you could call this function to save some memory.

清除缓存
清除 xhook 的缓存，重置所有的全局标示。
如果你确定你需要的所有 PLT 入口点都已经被替换了，你可以调用这个函数来释放和节省一些内存空间。

5. Enable/Disable debug info
void xhook_enable_debug(int flag);
Pass 1 to flag for enable debug info. Pass 0 to flag for disable. (disabled by default)
Debug info will be sent to logcat with tag xhook.

启用/禁用 调试信息
给 flag 参数传 1 表示启用调试信息，传 0 表示禁用调试信息。 (默认为：禁用)
调试信息将被输出到 logcat，对应的 TAG 为：xhook。

6. Enable/Disable SFP (segmentation fault protection)
void xhook_enable_sigsegv_protection(int flag);

Pass 1 to flag for enable SFP. Pass 0 to flag for disable. (enabled by default)

xhook is NOT a compliant business layer library. We have to calculate the value of some pointers directly. Reading or writing the memory pointed to by these pointers will cause a segmentation fault in some unusual situations and environment. The APP crash rate increased which caused by xhook is about one ten-millionth (0.0000001) according to our test. (The increased crash rate is also related to the ELFs and symbols you need to hook). Finally, we have to use some trick to prevent this harmless crashing. We called it SFP (segmentation fault protection) which consists of: sigaction(), SIGSEGV, siglongjmp() and sigsetjmp().

You should always enable SFP for release-APP, this will prevent your app from crashing. On the other hand, you should always disable SFP for debug-APP, so you can't miss any common coding mistakes that should be fixed.

 启用/禁用 SFP (段错误保护)
 给 flag 参数传 1 表示启用 SFP，传 0 表示禁用 SFP。 (默认为：启用)

xhook 并不是一个常规的业务层的动态库。在 xhook 中，我们不得不直接计算一些内存指针的值。在一些极端的情况和环境下，读或者写这些指针指向的内存会发生段错误。根据我们的测试，xhook 的行为将导致 APP 崩溃率增加 “一千万分之一” (0.0000001)。（具体崩溃率可能会增加多少，也和你想要 hook 的库和符号有关）。最终，我们不得不使用某些方法来防止这些无害的崩溃。我们叫它SFP (段错误保护)，它是由这些调用和值组成的：sigaction()， SIGSEGV， siglongjmp() 和 sigsetjmp()。

在 release 版本的 APP 中，你应该始终启用 SFP，这能防止你的 APP 因为 xhook 而崩溃。在 debug 版本的 APP 中，你应该始终禁用 SFP，这样你就不会丢失那些一般性的编码失误导致的段错误，这些段错误是应该被修复的。


```
