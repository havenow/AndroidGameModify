import frida
import sys
import io


device = frida.get_usb_device()

session = device.attach(int(sys.argv[1]))

scr = """
function hookTest(){
    var funcAddr = Module.findExportByName("libhookDemo.so", "_Z15native_testHookP7_JNIEnvP8_jobject");//Java_com_test_app_NativeHelper_helloFromC
    console.log(funcAddr);
    if(funcAddr != null){
        Interceptor.attach(funcAddr,{
            onEnter: function(args){

            },
            onLeave: function(retval){
                var env = Java.vm.tryGetEnv();
                var jstr = env.newStringUtf("test call jni func");  //主动调用jni函数 cstr转jstr
                retval.replace(jstr);
                var cstr = env.getStringUtfChars(jstr); //主动调用 jstr转cstr
                console.log(cstr.readCString());
                console.log(hexdump(cstr));
            }
        });
    }
}


function main(){
	hookTest();
}

setImmediate(main);
"""

def on_message(message ,data):
	if message['type'] == 'error':
		print(message['description'])
	print(data)


script = session.create_script(scr)
script.on("message" , on_message)
script.load()
sys.stdin.read()