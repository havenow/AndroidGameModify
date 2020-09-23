import frida
import sys
import io


device = frida.get_usb_device()

session = device.attach("com.example.hellojni")

scr = """
function hookTest32(){
    var Addr1 = Module.findExportByName("libc.so", "dlopen");
    console.log("libc.so dlopen addr:	" + Addr1);
    if(Addr1 != null){
        Interceptor.attach(Addr1,{
            onEnter: function(args){
				console.log("dlopen---begin")
               
            },
            onLeave: function(retval){
				
				console.log("dlopen---end  \\n\\n")
            }
        });
    } 
	
	var Addr2 = Module.findExportByName("libc.so", "dlsym");
    console.log("libc.so dlsym addr:	" + Addr2);
    if(Addr2 != null){
        Interceptor.attach(Addr2,{
            onEnter: function(args){
				console.log("dlsym---begin")
				console.log(args[1].readCString())
            },
            onLeave: function(retval){
				console.log(retval);
				console.log("dlsym---end  \\n\\n")
            }
        });
    } 
	
	var Addr3 = Module.findExportByName("libhello-jni.so", "JNI_OnLoad");
    console.log("libhello-jni.so JNI_OnLoad addr:	" + Addr3);
    if(Addr3 != null){
        Interceptor.attach(Addr3,{
            onEnter: function(){
				console.log("JNI_OnLoad---begin")
               
            },
            onLeave: function(retval){
				
				console.log("JNI_OnLoad---end  \\n\\n")
            }
        });
    } 
	
	var Addr4 = Module.findExportByName("libhello-jni.so", "native_hello");//如果是在load so之前获取的地址是null
    console.log("libhello-jni.so native_hello addr:	" + Addr4);
    if(Addr4 != null){
        Interceptor.attach(Addr4,{
            onEnter: function(){
				console.log("native_hello---begin")
               
            },
            onLeave: function(retval){
				
				console.log("native_hello---end  \\n\\n")
            }
        });
    } 
	
	var Addr5 = Module.findExportByName("libhello-jni.so", "Java_com_dsemu_drastic_DraSticActivity_stringFromJNI");
    console.log("libhello-jni.so Java_com_dsemu_drastic_DraSticActivity_stringFromJNI addr:	" + Addr5);
    if(Addr5 != null){
        Interceptor.attach(Addr5,{
            onEnter: function(){
				console.log("Java_com_dsemu_drastic_DraSticActivity_stringFromJNI---begin")
               
            },
            onLeave: function(retval){
				
				console.log("Java_com_dsemu_drastic_DraSticActivity_stringFromJNI---end  \\n\\n")
            }
        });
    } 
	
	Java.perform(function(){
        var envAddr = ptr(Java.vm.tryGetEnv().handle).readPointer();
        var registerNativesAddr = envAddr.add(860).readPointer();//32位的so 216*4 - 4 = 860； 64位的so 216*8 - 8 = 1720
        console.log("registerNativesAddr", registerNativesAddr)
        if(registerNativesAddr != null){     //Hook registerNatives 获取动态注册的函数地址
			console.log("Interceptor.attach registerNatives")
            Interceptor.attach(registerNativesAddr,{
                onEnter: function(args){
                    console.log("registerNatives---begin")
					console.log(args[2].readPointer().readCString());
					console.log(args[2].add(Process.pointerSize).readPointer().readCString());
					console.log(args[2].add(Process.pointerSize * 2).readPointer());
                },
                onLeave: function(retval){
					console.log("registerNatives---end  \\n\\n")
                }
            });
        }

    });
}

function main(){
	hookTest32();
}

setImmediate(main);
"""

def on_message(message ,data):
   print('on_message')


script = session.create_script(scr)
script.on("message" , on_message)
script.load()
sys.stdin.read()