import frida
import sys
import io


device = frida.get_usb_device()

session = device.attach(int(sys.argv[1]))

scr = """

function hookTest(){
	var dlopen = Module.findExportByName(null, "dlopen");
    console.log(dlopen);
    if(dlopen != null){
        Interceptor.attach(dlopen,{
            onEnter: function(args){
                var soName = args[0].readCString();
                console.log('dlopen ## ', soName);
                if(soName.indexOf("libhookDemo.so") != -1){
                    this.hook = true;
                }
            },
            onLeave: function(retval){
                if(this.hook) { 
					console.log('dlopen libhookDemo## Module.findBaseAddress | soAddr.add | Interceptor.attach');
				}
            }
        });
    }

    var android_dlopen_ext = Module.findExportByName(null, "android_dlopen_ext");
    console.log(android_dlopen_ext);
    if(android_dlopen_ext != null){
        Interceptor.attach(android_dlopen_ext,{
            onEnter: function(args){
                var soName = args[0].readCString();
                console.log('android_dlopen_ext ## ', soName);
                if(soName.indexOf("libhookDemo.so") != -1){
                    this.hook = true;
                }
            },
            onLeave: function(retval){
                if(this.hook) { 
					console.log('android_dlopen_ext libhookDemo## envAddr.add(0x538) envAddr.add(1720) | Interceptor.attach(newStringUtfAddr | Interceptor.attach(registerNativesAddr');
				}
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
   print('on_message')


script = session.create_script(scr)
script.on("message" , on_message)
script.load()
sys.stdin.read()