import frida
import sys
import io


device = frida.get_usb_device()

session = device.attach(int(sys.argv[1]))

scr = """
function hookTest(){
    var soAddr = Module.findBaseAddress("libhookDemo.so");
    console.log(soAddr);
    var funcAddr = soAddr.add(0x23F4); //函数地址计算 thumb+1 ARM不加
    console.log(funcAddr);

    if(funcAddr != null){
        Interceptor.attach(funcAddr,{
            onEnter: function(args){

            },
            onLeave: function(retval){
                console.log(hexdump(retval));
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