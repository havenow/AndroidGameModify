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
在夜神模拟上面通过so方式ptrace(PTRACE_ATTACH会失败，没找到原因（如果将程序放在virtualxposed中运行，PTRACE_ATTACH是成功的，应该还是权限的问题）            
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
	使用elf，/data/local/tmp目下调用ptrace：
     root的手机  uid = 2000，用adb shell ps -ef查看，uid是shell
     夜神模拟    uid = 0
	 uid为0时ptrace(PTRACE_DETACH,才会成功
	 
	在Virtual Xposed中使用so调用ptrace，是可以成功的，但是无法看到日志。
```

# linux内存
text、rodata、data、bss、stack、heap     
https://www.cnblogs.com/DataArt/p/9879192.html     

.text (r-x), .rodata (r--), .data (rw-) and .bss (rw-)     

/proc/${pid}/maps，这个文件有内存所有的详细信息     
查找的内存段：     
heap, stack, .data, .bss     

```
 /*
             * get the load address for regions of the same ELF file
             *
             * When the ELF loader loads an executable or a library into
             * memory, there is one region per ELF segment created:
             * .text (r-x), .rodata (r--), .data (rw-) and .bss (rw-). The
             * 'x' permission of .text is used to detect the load address
             * (region start) and the end of the ELF file in memory. All
             * these regions have the same filename. The only exception
             * is the .bss region. Its filename is empty and it is
             * consecutive with the .data region. But the regions .bss and
             * .rodata may not be present with some ELF files. This is why
             * we can't rely on other regions to be consecutive in memory.
             * There should never be more than these four regions.
             * The data regions use their variables relative to the load
             * address. So determining it makes sense as we can get the
             * variable address used within the ELF file with it.
             * But for the executable there is the special case that there
             * is a gap between .text and .rodata. Other regions might be
             * loaded via mmap() to it. So we have to count the number of
             * regions belonging to the exe separately to handle that.
             * References:
             * http://en.wikipedia.org/wiki/Executable_and_Linkable_Format
             * http://wiki.osdev.org/ELF
             * http://lwn.net/Articles/531148/
             */
```

# 连接符
```
##表示连接符
#define DEFINE_INTEGER_MATCHANY_ROUTINE(DATATYPENAME, DATAWIDTH) \
    int scan_routine_##DATATYPENAME##_ANY ()//DATATYPENAME的后面跟的是字符串,所以需要加##
	{
	}

DEFINE_INTEGER_MATCHANY_ROUTINE(INTEGER8, 8)
展开之后就是：
    int scan_routine_INTEGER8_ANY ()
	{
	}

注意下面的区别

#define REGISTER_NATIVE(env, module) { \
	extern int register_##module(JNIEnv *); \
	if (register_##module(env) < 0) \
		return JNI_FALSE; \
}

module后面不是跟的字符串，所以不需要加##

REGISTER_NATIVE(env, Emulator);会替换成
//##表示连接符
{
	extern int register_Emulator(JNIEnv *);//register_Emulator(JNIEnv *)在emulator.cpp中
	if (register_Emulator(env) < 0)
		return JNI_FALSE;
}
```

# (stringType " ")

```
#define FLAG_MACRO_NEW(stringType) true ? (stringType " ") : ""
printf("%s\n", FLAG_MACRO_NEW("I32"));

打印：I32 (注意有一个空格)

#define FLAG_MACRO_NEW(stringType) true ? (stringType " aditext") : ""
printf("%s\n", FLAG_MACRO_NEW("I32"));

打印：I32 aditext

```

# char data[0]	data是一个数组名；该数组没有元素

先看一段代码，old_value_and_match_info data[0];没有暂用内存空间    
```
typedef struct {
	uint8_t old_value;
	match_flags match_info;
} old_value_and_match_info;//8字节

typedef struct {
	void *first_byte_in_child;
	unsigned long number_of_bytes;
	old_value_and_match_info data[0];
} matches_and_old_values_swath;//8字节
```

```
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
typedef struct _Info
{
    int i;
    char data[0];
}Info;
int main(int argc, char* argv[])
{
    char buf[10] = "123456789";
    void* p = NULL;
    printf("%d/n",sizeof(Info));
    Info* info = (Info*)malloc(sizeof(Info) + 10);
    p = (void*)info->data;
    printf("addr of info is %p. addr of data is %p ./n", info, p);
    strcpy((char*)p, buf);
    printf("%s/n", (char*)p);
    return 0;
}

data是一个数组名；该数组没有元素；该数组的真实地址紧随结构体Info之后；这种声明方法可以巧妙的实现C语言里的数组扩展。    

输出结果：
4
addr of info is 0072B1D0. addr of data is 0072B1D4 .
123456789
```

# /proc/$pid/mem

```
/proc/$pid/mem以与进程中相同的方式显示$pid的内存映射,即伪文件中偏移x处的字节与进程中地址x处的字节相同.
如果在该过程中未映射地址,则从文件中的相应偏移量读取将返回EIO(输入/输出错误).
例如,由于进程中的第一个页面从未映射(因此取消引用NULL指针会干净地失败而不是无意中访问实际内存),读取/proc/$pid/mem的第一个字节总会产生I/O错误.

找出进程内存映射的哪些部分的方法是读取/proc/$pid/maps.此文件包含每个映射区域一行,如下所示：

08048000-08054000 r-xp 00000000 08:01 828061     /bin/cat
08c9b000-08cbc000 rw-p 00000000 00:00 0          [heap]

前两个数字是区域的边界(第一个字节的地址和最后一个字节,以hexa表示).
下一列包含权限,如果这是文件映射,则有关于文件(偏移量,设备,inode和名称)的一些信息.
有关更多信息,请参见proc(5)手册页或Understanding Linux /proc/id/maps.
```

# crash

```

result->int64_value =    *((int64_t *)&peekbuf.cache[reqaddr - peekbuf.base]);  /*lint !e826 */

原因：
在夜神模拟器中要使用x86架构的程序，如果使用armeabi-v7a架构的程序会崩溃
```

# 读取内存数据

```
//-----------------		searchregions
void *address = NULL;
unsigned offset, nread = 0;
for (offset = 0; offset < nread; offset++) 
{
	address = r->start + offset;
	
	peekdata(vars->target, address, &data_value) //第一次地址：0x8000	第二次地址：0x8001	
}


//-----------------		peekdata
for (i = 0; i < shift_size1; i += sizeof(long))// 循环一次到两次
{
	long ptraced_long = ptrace(PTRACE_PEEKDATA, pid, ptrace_address, NULL);	//读4字节
	
	... ....
	*((long *)&peekbuf.cache[peekbuf.size]) = ptraced_long;
    peekbuf.size += sizeof(long);
}
	
result->int64_value =    *((int64_t *)&peekbuf.cache[reqaddr - peekbuf.base]);  /*lint !e826 */	
//结果存的是8字节
//第一次	result->int64_value = *((int64_t *)&peekbuf.cache[0]);
//第二次	result->int64_value = *((int64_t *)&peekbuf.cache[1]);
```

# 输出日志到文件

```
#include <stdarg.h>
void appendToLogFile(const char *format, ...)
{
	char content[1024];
	memset(content, 0, 1024);

	va_list args;
	va_start(args, format);
	vsprintf(content, format, args);
#if 1	//添加换行
	strcat(content, "\n");
#endif 	
	va_end(args);

	FILE* log_file = fopen("/data/local/tmp/scan.log", "a+");
	if (log_file != NULL) {
		fwrite(content, strlen(content), 1, log_file);
		fflush(log_file);
		fclose(log_file);
	}
}

使用：    
appendToLogFile("## tag2.1.3 peekbuf.base : %0x", peekbuf.base);
```

# 内存中数据代表不同数据类型时的值
在内存中存放的数据是不会区分类型的，在某个地址取出来的数据，强制转换之后数据是可以直观理解的数据     
[float double类型,在内存中的存储方式](https://github.com/havenow/AndroidGameModify/blob/master/float%20double%E7%B1%BB%E5%9E%8B%2C%E5%9C%A8%E5%86%85%E5%AD%98%E4%B8%AD%E7%9A%84%E5%AD%98%E5%82%A8%E6%96%B9%E5%BC%8F.md)     

```
	unsigned char uip_buf1[] = { 0x5E, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00 };//内存中的十六进制数据
	char* 	resultChar		= (char*)(&(uip_buf1));		//取出1字节
	int* 	resultInt		= (int*)(&(uip_buf1));		//取出4字节
	float*  resultFlo		= (float*)(&(uip_buf1));	//取出4字节
	double* resultDou		= (double*)(&(uip_buf1));	//取出8字节
	printf("%c\n", *resultChar);												//^(对应ascii码0x5E)
	printf("%d\n", *resultInt);													//94
	printf("%f %lf %e %.8e\n", *resultFlo, *resultFlo, *resultFlo, *resultFlo);	//0.000000 0.000000 1.317221e-43  1.31722056e-43
	printf("%f %lf %e %.8e\n", *resultDou, *resultDou, *resultDou, *resultDou);	//0.000000 0.000000 4.243992e-313 4.24399159e-313
	
	char strMsg[512];
	memset(strMsg, 0, 512);
	sprintf(strMsg, "%c, %d, %.8e, %.8e\n", *resultChar, *resultInt, *resultFlo, *resultDou);
	printf("%s", strMsg);														//^, 94, 1.31722056e-43, 4.24399159e-313

```

# long和int的区别

```
long类型表示long int，一般简写为long，注意long不表示long double。
虽然有时候会有sizeof(long)=sizeof(int)，long int与int是不同的：

16位系统：long是4字节，int是2字节
32位系统：long是4字节，int是4字节
64位系统：long也是4字节，int是4字节

标准只规定long不小于int的长度，int不小于short的长度。

即sizeof(long)>=sizeof(int)
```

# ptrace获取到内存数据存储到文件中

```
#include <stdarg.h>
void dumpMem(long data)
{
	FILE* log_file = fopen("/data/local/tmp/memRegion", "a+");
	if (log_file != NULL) 
	{
		fwrite(&data, 1, 4, log_file);
		fflush(log_file);
		fclose(log_file);
	}
}

long ptraced_long = ptrace(PTRACE_PEEKDATA, pid, ptrace_address, NULL);		//ptrace获取到的数据是long类型的
dumpMem(ptraced_long);

```

# 获取内存块里面的数据
[内存块0x8000-0xA000数据的读取](https://github.com/havenow/AndroidGameModify/blob/master/%E5%86%85%E5%AD%98%E5%9D%970x8000-0xA000%E6%95%B0%E6%8D%AE%E7%9A%84%E8%AF%BB%E5%8F%96.log)      

```
element_t *n = vars->regions->head;
region_t *r;
while (n) 
{
	unsigned offset, nread = 0;
	r = n->data;
	nread = r->size;
	
	for (offset = 0; offset < nread; offset++) 
	{
		value_t data_value;
		address = r->start + offset;
		
		peekdata(vars->target, address, &data_value);
	}
}

bool peekdata(pid_t pid, void *addr, value_t * result)
{
	if ()//有缓存时，直接读取 返回
	{
		case 1;
		
		result->int64_value =    *((int64_t *)&peekbuf.cache[reqaddr - peekbuf.base]); 
		return true;
	}
	else if ()//没有缓存，计算shift_size1 后面ptrace(PTRACE_PEEKDATA
	{
		case 2;
		//做数据的迁移
	}
	else if ()//开始时调用一次
	{
		case 3;
	}
	for (i = 0; i < shift_size1; i += sizeof(long))
    {
		long ptraced_long = ptrace(PTRACE_PEEKDATA, pid, ptrace_address, NULL);
	}
	
	result->int64_value =    *((int64_t *)&peekbuf.cache[reqaddr - peekbuf.base]); 
	return true;
}

读内存
	case 3
	
	case 3
	case 2
	case 1
	case 1
	case 1
	case 2
	case 1
	case 1
	case 1
	case 2
	case 1
	case 1
	case 1
	... ...
```

# root的手机ptrace(PTRACE_ATTACH )失败
在调用ptrace(PTRACE_ATTACH )之前，需要先执行su，有管理权限调用才会成功。     

```
	if (system("su") == -1)
		LOGE( "system(\"su\") failed, command was not executed.\n");
	else
		LOGE("system(\"su\") sucess, command was executed.\n");
		
	Android Studio下面调试（ndk）上面的代码，调用system("su")是会signal---SIGCHLD
```

# ptrace的PTRACE_ATTACH和PTRACE_DETACH要成对调用
ptrace只调用PTRACE_ATTACH游戏后，回到游戏，游戏会卡死黑屏。      

# RegisterNatives (IIII)I

```
#define APP_PKG_NAME	"com/gameassist/plugin/nativeutils"
# define NELEM(x) ((int) (sizeof(x) / sizeof((x)[0])))
int register_GodBridge(JNIEnv *env)
{
	static const JNINativeMethod methods[] = {
			{ "init", "(I)V",(void *)init_bridge}, 
			{ "initNativePtr", "(II)V", (void *)initNativePtr_bridge }, 
			{ "nativeProcessCheat", "(IIII)I", (int *)nativeProcessCheat_bridge }
	};

    jclass clazz;
    clazz = env->FindClass(APP_PKG_NAME"/NativeUtils");
    return env->RegisterNatives(clazz, methods, NELEM(methods));
}

注意"(IIII)I"，括号里面的IIII直接不要加分号，不然会register失败
Ljava/lang/String;后面是要加分号的
```
# dlsym extern "C"
加了extern "C"	才能通过dlsym找到so里面的函数    
```
extern "C"
{
__attribute__((visibility("default"))) int jniInitNativePtr(JNIEnv *env, jobject thiz, int arg0, int arg1)
{
    LOGE("#########	jniInitNativePtr begin.\n");

 
    LOGE("#########	jniInitNativePtr end.\n");
	return 0;
}

__attribute__((visibility("default"))) int jniDoProcessCheat(JNIEnv *env, jobject thiz, int arg0, int arg1, int arg2, int arg3)
{
    LOGE("#########	jniDoProcessCheat begin.\n");

   
    LOGE("#########	jniDoProcessCheat end.\n");
	return 0;
}

}
```

# dlsym JNI_OnLoad

```
int (*f_JNIOnLoad)(JavaVM* vm, void* reserved);

f_JNIOnLoad = (jint(*)(JavaVM*, void*))dlsym(dll, "JNI_OnLoad");

JNI_OnLoad的实现都是这样写：
__attribute__((visibility("default")))
jint JNI_OnLoad(JavaVM* vm, void* reserved)
{

}
```

# dlsym jniInitNativePtr失败的原因

```
so在IDA中export可以看到的函数都是可以dlysm的，在Functions window中看到的函数名和export的可能是不一样的

f_initNativePtr = (int(*)(JNIEnv *, jobject, unsigned int, unsigned int))dlsym(dll, "_Z16jniInitNativePtrP7_JNIEnvP7_jclassii");
f_nativeProcessCheat = (int(*)(JNIEnv *, jobject, int, int, int, int))dlsym(dll, "_Z17jniDoProcessCheatP7_JNIEnvP7_jclassiiii"); 


//info.dli_fbase 加载so时的地址，info.dli_saddr 符号的实际地址
//info.dli_fbase + 查看IDA .text段 符号的偏移 = 符号的实际地址
	Dl_info info;
	dladdr((void*)f_JNIOnLoad, &info);
	appendToLogFile("f_JNIOnLoad          Dl_info: %x	%x	%x", info.dli_fbase, info.dli_saddr, (void*)(uint32_t(info.dli_saddr) - uint32_t(info.dli_fbase)));
	dladdr((void*)f_initNativePtr, &info);
	appendToLogFile("f_initNativePtr      Dl_info: %x	%x	%x", info.dli_fbase, info.dli_saddr, (void*)(uint32_t(info.dli_saddr) - uint32_t(info.dli_fbase)));
	dladdr((void*)f_nativeProcessCheat, &info);
	appendToLogFile("f_nativeProcessCheat Dl_info: %x	%x	%x", info.dli_fbase, info.dli_saddr, (void*)(uint32_t(info.dli_saddr) - uint32_t(info.dli_fbase)));

	(void*)(uint32_t(info.dli_saddr) - uint32_t(info.dli_fbase))在armeabi-v7a x86下可以编译通过， 在x86_64下编译不报错
	error: 'Dl_info' was not declared in this scope Dl_info info;	在Application.mk中添加APP_PLATFORM := android-9

打印的日志：
#########	init_bridge begin.
dlsym dlopen sucess
dlsym dll != NULL
find f_JNIOnLoad.
call f_JNIOnLoad.
find jniInitNativePtr.
find jniDoProcessCheat.
f_JNIOnLoad          Dl_info: c4da0000	c4da539d	539d
f_initNativePtr      Dl_info: c4da0000	c4da5301	5301
f_nativeProcessCheat Dl_info: c4da0000	c4da52b9	52b9
#########	init_bridge end.
```

# unknown type name 'Dl_info' 
Dl_info info; 需要在Application.mk中使用APP_PLATFORM := android-9指定android平台版本。     
默认情况下，如果使用android-3并失败，出现找不到Dl_info     

