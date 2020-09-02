import frida
import sys
import io


device = frida.get_usb_device()

session = device.attach(int(sys.argv[1]))

scr = """
function hookTest(){
	var soAddr = Module.findBaseAddress("libhookDemo.so");
    console.log(soAddr);

     var sub_208C = soAddr.add(0x208C); //函数地址计算 thumb+1 ARM不加
     console.log(sub_208C);
     if(sub_208C != null){
        Interceptor.attach(sub_208C,{
            onEnter: function(args){
                this.args1 = args[1];		//args[1]是以指针的形式作为返回值  this.arg1的名称随意
            },
            onLeave: function(retval){
                console.log(hexdump(this.args1));	//输出指针参数返回值
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