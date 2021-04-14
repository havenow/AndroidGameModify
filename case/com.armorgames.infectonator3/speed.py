import frida
import sys
import io


device = frida.get_usb_device()

session = device.attach("com.armorgames.infectonator3")

scr = """
function hookTest(){
    var soAddr = Module.findBaseAddress("libil2cpp.so");
    console.log(soAddr);

    var funcAddr = soAddr.add(0x18DD71C); //函数地址计算 thumb+1 ARM不加	
    console.log(funcAddr);
	
	var count = 0;

    if(funcAddr != null){
		console.log("attach");
        Interceptor.attach(funcAddr,{
		
		
            onEnter: function(args){
				console.log("onEnter ---------------------------------<");
				//console.log(Thread.backtrace(this.context, Backtracer.ACCURATE).map(DebugSymbol.fromAddress).join('\\n') + '\\n');
				//console.log(args[0]);
            },
            onLeave: function(retval){
				count = count +1;
				//console.log(retval.toInt32()*count);
				//retval.replace(retval.toInt32()*count*100);	
				//if (retval != 0)
                //console.log("onLeave---------------------------------");
				//console.log(Thread.backtrace(this.context, Backtracer.ACCURATE).map(DebugSymbol.fromAddress).join('\\n') + '\\n');
				//console.log(retval);
				//console.log("onLeave--------------------------------->\\n");
				//console.log('retval  :' + retval.toInt32());
				//retval.replace(1557644296);													
				//console.log('retval replace :' + retval.toInt32());		
				//return retval;
            }
        });
     }
	
}

//void *(*IL2CPP_RUN_TIME_INVOKE)(void *method, void *obj, void **params, void **exc);

//typedef void *(*IL2CPP_METHOD_GET_CLASS)(void *method);
//typedef void *(*IL2CPP_CLASS_GET_IMAGE)(void *class_object);
//typedef void *(*IL2CPP_CLASS_FROM_NAME)(void *image, char *space, char *name);
//typedef char *(*IL2CPP_METHOD_FROM_NAME)(void * class_object, char *name, int agrs);

function hookTest1(){
	var addr_il2cpp_method_get_class =	Module.findExportByName("libil2cpp.so", "il2cpp_method_get_class");
	var addr_il2cpp_class_get_image =	Module.findExportByName("libil2cpp.so", "il2cpp_class_get_image");	
	var addr_il2cpp_class_from_name =	Module.findExportByName("libil2cpp.so", "il2cpp_class_from_name");	
	var addr_il2cpp_method_from_name =	Module.findExportByName("libil2cpp.so", "il2cpp_class_get_method_from_name");	

	var fun_il2cpp_method_get_class =			new NativeFunction(addr_il2cpp_method_get_class,	"pointer", ["pointer"]);	
	var fun_il2cpp_class_get_image =			new NativeFunction(addr_il2cpp_class_get_image,		"pointer", ["pointer"]);	
	var fun_il2cpp_class_from_name =			new NativeFunction(addr_il2cpp_class_from_name,		"pointer", ["pointer", "pointer", "pointer"]);
	var fun_il2cpp_class_get_method_from_name = new NativeFunction(addr_il2cpp_method_from_name,	"pointer", ["pointer", "pointer", "int"]);
	
    var addr_il2cpp_runtime_invoke = Module.findExportByName("libil2cpp.so", "il2cpp_runtime_invoke");
    console.log(addr_il2cpp_runtime_invoke);
    if(addr_il2cpp_runtime_invoke != null){
        Interceptor.attach(addr_il2cpp_runtime_invoke,{
            onEnter: function(args){
				//console.log("--------------------------------------------------------------------begin!!!")
				
					
                	var any_class = fun_il2cpp_method_get_class(args[0].readPointer());//void *method
					if (any_class != null) {
						//console.log("any_class != null")
						var image_name = fun_il2cpp_class_get_image(any_class);//void *class_object
						if (image_name != null) {
							//console.log("image_name != null");
							var space = Memory.allocUtf8String("UnityEngine");
							var name = Memory.allocUtf8String("Time");
							//var target_class = fun_il2cpp_class_from_name(image_name, space, name);//void *image, char *space, char *name 调用之后会卡住
							/*if (target_class != null) {
								console.log("target_class != null");
								var fname = Memory.allocUtf8String("set_timeScale");
								var il2cpp_time_scale_method = fun_il2cpp_class_get_method_from_name(target_class, fname, 1);
								console.log("called fun_il2cpp_class_get_method_from_name");
								if (il2cpp_time_scale_method != null) {
									console.log("il2cpp_time_scale_method != null");
								}
								else
									console.log("il2cpp_time_scale_method == null");
							}*/
						}
					}
					/*if (any_class != null) {
						console.log("any_class != null")
						var image_name = il2cpp_class_get_image(any_class);
						if (image_name != null) {
							var target_class = il2cpp_class_from_name(image_name, "UnityEngine", "Time");
							if (target_class != null) {
								var il2cpp_time_scale_method = il2cpp_method_from_name(target_class, "set_timeScale", 1);
								if (il2cpp_time_scale_method != null) {
									void *args[1];
									float scale = speed;
									args[0] = &scale;
									IL2CPP_RUN_TIME_INVOKE(il2cpp_time_scale_method, null, args, null);
								} 
							} 
						} 
					} */
            },
            onLeave: function(retval){
				//console.log("--------------------------------------------------------------------end  !!!\\n\\n")
            }
        });
    } 
}

function main(){
	//hookTest();
	hookTest1();
}

setImmediate(main);
"""

def on_message(message ,data):
   print('on_message')


script = session.create_script(scr)
script.on("message" , on_message)
script.load()
sys.stdin.read()