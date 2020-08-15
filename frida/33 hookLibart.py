import frida
import sys
import io


device = frida.get_usb_device()

session = device.attach(int(sys.argv[1]))

scr = """
function hookTest(){
	var artSym = Module.enumerateSymbols("libart.so");
    var NewStringUTFAddr = null;
    for(var i = 0; i < artSym.length; i++){
        if(artSym[i].name.indexOf("CheckJNI") == -1 && artSym[i].name.indexOf("NewStringUTF") != -1){
            console.log(JSON.stringify(artSym[i]));
            NewStringUTFAddr = artSym[i].address;
        }
    };

    if(NewStringUTFAddr != null){
        Interceptor.attach(NewStringUTFAddr,{
            onEnter: function(args){
                console.log(args[1].readCString());
            },
            onLeave: function(retval){

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