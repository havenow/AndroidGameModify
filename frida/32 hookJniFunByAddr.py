import frida
import sys
import io


device = frida.get_usb_device()

session = device.attach(int(sys.argv[1]))

scr = """
function hookTest(){
    Java.perform(function(){
        //console.log(JSON.stringify(Java.vm.tryGetEnv()));
        var envAddr = ptr(Java.vm.tryGetEnv().handle).readPointer();
        var newStringUtfAddr = envAddr.add(0x538).readPointer();
        var registerNativesAddr = envAddr.add(1720).readPointer();
        console.log("newStringUtfAddr", newStringUtfAddr);
        console.log("registerNativesAddr", registerNativesAddr)
        if(newStringUtfAddr != null){
            Interceptor.attach(newStringUtfAddr,{
                onEnter: function(args){
                    console.log(args[1].readCString());
                },
                onLeave: function(retval){

                }
            });
        }
        if(registerNativesAddr != null){     //Hook registerNatives获取动态注册的函数地址
            Interceptor.attach(registerNativesAddr,{
                onEnter: function(args){
                    if(args[2].readPointer().readCString() == "encode"){
                        console.log(args[2].readPointer().readCString());
                        console.log(args[2].add(Process.pointerSize).readPointer().readCString());
                        console.log(args[2].add(Process.pointerSize * 2).readPointer());
                        console.log('CCCryptorCreate called from:\\n' +
                        Thread.backtrace(this.context, Backtracer.FUZZY)
                        .map(DebugSymbol.fromAddress).join('\\n') + '\\n');
                    }
                    //console.log(hexdump(args[2]));
                    //console.log("sub_289C", Module.findBaseAddress("libhookDemo.so").add(0x289C));
                },
                onLeave: function(retval){

                }
            });
        }

    });
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