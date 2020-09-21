# 结论
```
下面提取了对jni开发有帮助的
java端调用System.loadLibrary()后，
1、dlopen so
2、dlsym so中的JNI_OnLoad函数，并且调用JNI_OnLoad函数
3、如果JNI_OnLoad函数中有registerNatives，对本地方法进行注册，这里是不会调用dlsym的，因为是将函数指针保存起来了
https://blog.csdn.net/Saintyyu/article/details/90452826
4、如果有Java_com.xxxx_fun的jni函数，在调用jni函数时，vm会dlysm jni函数，获取到jni函数地址，在调用jni函数，只会dlsym一次，
因为获取到jni函数的地址后，就不用在dlsym了

所以用registerNatives的方式比用Java_com.xxxx_fun的方式要快

https://www.jianshu.com/p/5252b62aa9d2


```

# registerNatives
```
JNINativeMethod nm;
nm.name = "g";
/* method descriptor assigned to signature field */
nm.signature = "()V";
nm.fnPtr = g_impl;
(*env)->RegisterNatives(env, cls, &nm, 1);

fnPtr是函数的地址，所以用RegistetNatives注册后的本地方法是不会去调用dlsym的


注意第三个值是取地址，获取函数的地址
System.c
/* Only register the performance-critical methods */
static JNINativeMethod methods[] = {
    {"currentTimeMillis", "()J",              (void *)&JVM_CurrentTimeMillis},
    {"nanoTime",          "()J",              (void *)&JVM_NanoTime},
    {"arraycopy",     "(" OBJ "I" OBJ "II)V", (void *)&JVM_ArrayCopy},
};

#undef OBJ

JNIEXPORT void JNICALL
Java_java_lang_System_registerNatives(JNIEnv *env, jclass cls)
{
    // 注册本地方法
    (*env)->RegisterNatives(env, cls,
                            methods, sizeof(methods)/sizeof(methods[0]));
}
```

# frida hook registerNatives函数 
```
Java.perform(function(){
        var envAddr = ptr(Java.vm.tryGetEnv().handle).readPointer();
        var registerNativesAddr = envAddr.add(860).readPointer();//32位的so 216*4 - 4 = 860； 64位的so 216*8 - 8 = 1720
        console.log("registerNativesAddr", registerNativesAddr)
        if(registerNativesAddr != null){     													//Hook registerNatives 获取动态注册的函数地址
			console.log("Interceptor.attach registerNatives")
            Interceptor.attach(registerNativesAddr,{
                onEnter: function(args){
                    console.log("registerNatives---begin")
					console.log(args[2].readPointer().readCString());							//打印函数的name
					console.log(args[2].add(Process.pointerSize).readPointer().readCString());	//打印
					console.log(args[2].add(Process.pointerSize * 2).readPointer());			//打印函数的地址
                },
                onLeave: function(retval){
					console.log("registerNatives---end  \\n\\n")
                }
            });
        }

    });

对于registerNatives的地址是基于env去做偏移的
查看 jni.h的struct JNINativeInterface {	
里面的变量都是指针类型，只用找到registerNatives的偏移即可，看到是216 -1
	//32位的so 216*4 - 4 = 860； 64位的so 216*8 - 8 = 1720
	
或者用IDA打开so，查看JNI_OnLoad中registerNatives做的偏移的值
signed int __fastcall sub_DDC(int a1, int a2, int a3, int a4)
{
  int v4; // ST08_4
  int v7; // [sp+0h] [bp-1Ch]
  int v8; // [sp+4h] [bp-18h]
  int v9; // [sp+Ch] [bp-10h]
  int v10; // [sp+14h] [bp-8h]

  v9 = a1;
  v4 = a2;
  v8 = a3;
  v7 = a4;
  _android_log_print(6, "testDlsym", "111111111111");
  v10 = (*(int (__fastcall **)(int, int))(*(_DWORD *)v9 + 24))(v9, v4);
  _android_log_print(6, "testDlsym", "22222222222");
  if ( !v10 )
    return 0;
  _android_log_print(6, "testDlsym", "33333333333");
  if ( (*(int (__fastcall **)(int, int, int, int))(*(_DWORD *)v9 + 860))(v9, v10, v8, v7) < 0 )			//这里的偏移是860， v9是env的地址
    return 0;
  _android_log_print(6, "testDlsym", "44444444");
  return 1;
}
```

# 由来
```
看雪上看到一篇文章，在不用改smali，重新打包的情况下，实现hook
如果可以这样，那对于有壳或者反调试的apk，这样就可以简单的实现hook了
但是这边尝试了下痛扁小朋友这个游戏，失败了，但是对jni的流程更加清晰了
https://bbs.pediy.com/thread-261844.htm

```


# 查到的一些点
```
1、查到的
System.loadLibrary(libName)
->Runtime.loadLibrary(libName,classLoader)
->Runtime.doLoad(libName,loader)
->Runtime.nativeLoad(name,loader,ldLibraryPath)
->JavaVMExt.loadNativieLibrary:(JavaVMExt* vm=Runtime::Current()->GetJavaVM();vm->LoadNativieLibrary(filename,classloader,detail))
->jni_internal.cc::LoadNativieLibrary->dlopen,dlsym
->JNI_OnLoad


2、查到的
System.loadLibrary(libName);
|--Runtime.loadLibrary0(libName,classLoader);
|--|--|--Runtime.nativeLoad(name,loader,ldLibraryPath);
|--|--|--Runtime_nativeLoad(JNIEnv* env, jclass, jstring javaFilename, jobject javaLoader, jstring javaLdLibraryPath);
|--|--|--|--JVM_NativeLoad(env, javaFilename, javaLoader);
|--|--|--|--JavaVMExt::LoadNativeLibrary(const std::string& path,Handle<mirror::ClassLoader> class_loader,std::string* detail)		
|--|--|--|--|--android::OpenNativeLibrary(dlopen);
|--|--|--|--|--|--SharedLibrary.FindSymbol;
|--|--|--|--|--|--SharedLibrary.FindSymbolWithoutNativeBridge(dlsym);

3、查到的
./art/runtime/java_vm_ext.cc：
bool JavaVMExt::LoadNativeLibrary(JNIEnv* env,  
                                  const std::string& path,  
                                  jobject class_loader,  
                                  jstring library_path,  
                                  std::string* error_msg) 
{  
			......					  
	// Below we dlopen but there is no paired dlclose, this would be necessary if we supported  
    // class unloading. Libraries will only be unloaded when the reference count (incremented by  
    // dlopen) becomes zero from dlclose.  							  
								  
	void* handle = android::OpenNativeLibrary(env,  
                                            runtime_->GetTargetSdkVersion(),  
                                            path_str,  
                                            class_loader,  
                                            library_path);  
   
	bool needs_native_bridge = false;  
	if (handle == nullptr) {  
	if (android::NativeBridgeIsSupported(path_str)) {  												tag1	此函数里面会调用	dlopen 和 dlsym
		handle = android::NativeBridgeLoadLibrary(path_str, RTLD_NOW);  
		needs_native_bridge = true;  
	}  
	}  
	
	VLOG(jni) << "[Call to dlopen(\"" << path << "\", RTLD_NOW) returned " << handle << "]";  
	
	if (handle == nullptr) {  
	*error_msg = dlerror();  
	VLOG(jni) << "dlopen(\"" << path << "\", RTLD_NOW) failed: " << *error_msg;  
	return false;  
	}  			

	sym = library->FindSymbol("JNI_OnLoad", nullptr);  												tag2	找到	JNI_OnLoad (应该是用dlsym找到的)
	
	VLOG(jni) << "[Calling JNI_OnLoad in \"" << path << "\"]";  
    typedef int (*JNI_OnLoadFn)(JavaVM*, void*);  
    JNI_OnLoadFn jni_on_load = reinterpret_cast<JNI_OnLoadFn>(sym);  
    int version = (*jni_on_load)(this, nullptr);  													tag3	调用	JNI_OnLoad
	
	...
}


4、在so的JNI_OnLoad函数中打印出的堆栈
打印出来的堆栈
    backtrace:
    #00 pc 00004acd  /data/app/com.lakegame.dadnme-Dqh5EZjAUob4iJUGwNR5QA==/lib/arm/libCocos.so (_Z13callstackDumpRSs+00)
    #01 pc 00004b89  /data/app/com.lakegame.dadnme-Dqh5EZjAUob4iJUGwNR5QA==/lib/arm/libCocos.so (_Z15callstackLogcatiPKc+00)
    #02 pc 00004fad  /data/app/com.lakegame.dadnme-Dqh5EZjAUob4iJUGwNR5QA==/lib/arm/libCocos.so (JNI_OnLoad+00)
    #03 pc 0028ee35  /apex/com.android.runtime/lib/libart.so (_ZN3art9JavaVMExt17LoadNativeLibraryEP7_JNIEnvRKNSt3__112basic_stringIcNS3_11char_traitsIcEENS3_9allocatorIcEEEEP8_jobjectP7_jclassPS9_+00)
    #04 pc 000039a3  /apex/com.android.runtime/lib/libopenjdkjvm.so (JVM_NativeLoad+00)
	
结论：
java中System.loadLibrary so之后，从JavaVMExt::LoadNativeLibrary函数可以看出：会先调用dlopen dlsym，然后在进入so的JNI_OnLoad函数。
注意下面的描述
	// Below we dlopen but there is no paired dlclose, this would be necessary if we supported  
    // class unloading. Libraries will only be unloaded when the reference count (incremented by  
    // dlopen) becomes zero from dlclose.  	
所以在JNI_OnLoad函数继续多次dlopen 该so，返回的句柄都是一样的

```