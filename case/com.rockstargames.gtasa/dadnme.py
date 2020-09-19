import frida
import sys
import io


device = frida.get_usb_device()

session = device.attach("com.lakegame.dadnme")

scr = """
function hookTest32(){
    var helloAddr = Module.findExportByName("libc.so", "malloc");
    console.log("libc.so malloc addr:	" + helloAddr);
    if(helloAddr != null){
        Interceptor.attach(helloAddr,{
            onEnter: function(args){
				//console.log("---begin")
                //console.log(args[0]);
				if (args[0] == 0x1008)//64:0x1408 32:0x1008
				{
					console.log("---begin");
					console.log(args[0]);
					//console.log(Thread.backtrace(this.context, Backtracer.ACCURATE).map(DebugSymbol.fromAddress).join('\\n') + '\\n');
					this.argsTmp = args[0];
				}
				
				/*var ios = new File("/data/local/tmp/dadnme.txt", "w");
				ios.write("--------------------------------------------------------------------begin!!!\\n");
				ios.write(Thread.backtrace(this.context, Backtracer.ACCURATE)
				.map(DebugSymbol.fromAddress).join('\\n') + '\\n');
				ios.flush();
				ios.close();*/
            },
            onLeave: function(retval){
				if(this.argsTmp == 0x1008)//64:0x1408 32:0x1008
				{
					console.log(retval);
					
					var addr1 = 0xF44;
					var addr2 = 0xAA4;
					
					console.log("---blood value")
					var strByte = retval.add(addr1).readByteArray(4);//读内存 4字节
					console.log(strByte);
					//console.log(hexdump(retval.add(addr1)));//dump指定内存
					
					console.log("---crazy value")
					var strByte = retval.add(addr2).readByteArray(4);//读内存 4字节
					console.log(strByte);
					//console.log(hexdump(retval.add(addr2)));//dump指定内存
					
					
					console.log("---end  \\n\\n")
				}
                //console.log(retval);
				//console.log("---end  \\n\\n")
            }
        });
    } 
}

function hookTest64(){
    var helloAddr = Module.findExportByName("libc.so", "malloc");
    console.log("libc.so malloc addr:	" + helloAddr);
    if(helloAddr != null){
        Interceptor.attach(helloAddr,{
            onEnter: function(args){
				//console.log("---begin")
                //console.log(args[0]);
				if (args[0] == 0x1408)//64:0x1408 32:0x1008
				{
					console.log("---begin");
					console.log(args[0]);
					//console.log(Thread.backtrace(this.context, Backtracer.ACCURATE).map(DebugSymbol.fromAddress).join('\\n') + '\\n');
					this.argsTmp = args[0];
				}
				
				/*var ios = new File("/data/local/tmp/dadnme.txt", "w");
				ios.write("--------------------------------------------------------------------begin!!!\\n");
				ios.write(Thread.backtrace(this.context, Backtracer.ACCURATE)
				.map(DebugSymbol.fromAddress).join('\\n') + '\\n');
				ios.flush();
				ios.close();*/
            },
            onLeave: function(retval){
				if(this.argsTmp == 0x1408)//64:0x1408 32:0x1008
				{
					console.log(retval);
					
					var addr1 = 0x130C;
					var addr2 = 0xD44;
					
					console.log("---blood value")
					var strByte = retval.add(addr1).readByteArray(4);//读内存 4字节
					console.log(strByte);
					//console.log(hexdump(retval.add(addr1)));//dump指定内存
					
					console.log("---crazy value")
					var strByte = retval.add(addr2).readByteArray(4);//读内存 4字节
					console.log(strByte);
					//console.log(hexdump(retval.add(addr2)));//dump指定内存
					
					
					console.log("---end  \\n\\n")
				}
                //console.log(retval);
				//console.log("---end  \\n\\n")
            }
        });
    } 
}

function hookTest1(){
    var helloAddr = Module.findExportByName("libCocos.so", "_ZN8uirender21PottyRacer34Optimizer8saveSaveEv");
    console.log(helloAddr);
    if(helloAddr != null){
        Interceptor.attach(helloAddr,{
            onEnter: function(args){
				console.log("--------------------------------------------------------------------begin!!!")
                
            },
            onLeave: function(retval){
				console.log("--------------------------------------------------------------------end  !!!\\n\\n")
            }
        });
    } 
}

function hookSys(){
	var exports = Module.enumerateExports("libCocos.so");
	
	console.log('sys----------------');
	/*
	for(var i = 0; i < exports.length; i++){
		console.log(JSON.stringify(exports[i]));
		console.log(exports[i].address);
	}
	*/
	/*
	for(var i = 0; i < 7; i++){
		var addr = exports[i].address;
		if(addr != null){
        Interceptor.attach(addr,{
            onEnter: function(args){
				console.log("--------------------------------------------------------------------begin!!!")
                
            },
            onLeave: function(retval){
				console.log("--------------------------------------------------------------------end  !!!\\n\\n")
            }
        });
		} 
	}
	*/
	/*
	for(var i = 0; i < imports.length; i++){
		 Interceptor.attach(imports[i].address,{
            onEnter: function(){
				console.log("sys enter!!!")
                console.log(JSON.stringify(imports[i]));
            },
        });
		
	}
	*/
}

function main(){
	hookTest32();
	//hookTest64();
	//hookTest1();
	//hookSys();
}

setImmediate(main);
"""

def on_message(message ,data):
   print('on_message')


script = session.create_script(scr)
script.on("message" , on_message)
script.load()
sys.stdin.read()