import frida
import sys
import io


device = frida.get_usb_device()

session = device.attach(int(sys.argv[1]))

scr = """
function hookTest(){
    var helloAddr = Module.findExportByName("libhookDemo.so", "Java_com_test_add");
    console.log(helloAddr);
    if(helloAddr != null){
        Interceptor.attach(helloAddr,{
            onEnter: function(args){
                console.log(args[0]);
                console.log(args[1]);
                console.log(args[2]);
                console.log(args[3]);
                console.log(args[4].toInt32());
            },
            onLeave: function(retval){
                console.log(retval);
                console.log("retval", retval.toInt32());
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