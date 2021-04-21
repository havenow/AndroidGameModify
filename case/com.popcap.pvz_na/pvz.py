import frida
import sys
import io


device = frida.get_usb_device()

session = device.attach("com.popcap.pvz_na")

scr = """
function hookTest(){
    var soAddr = Module.findBaseAddress("libpvz.so");
    console.log(soAddr);

	//sub_13EF3C
    /*var funcAddr = soAddr.add(0x13EF3C); //函数地址计算 thumb+1 ARM不加	
    console.log(funcAddr);

    if(funcAddr != null){
		console.log("attach sub_13EF3C");
        Interceptor.attach(funcAddr,{
		
		
            onEnter: function(args){
				console.log("onEnter sub_13EF3C---------------------------------<");
				//console.log(Thread.backtrace(this.context, Backtracer.ACCURATE).map(DebugSymbol.fromAddress).join('\\n') + '\\n');
				//console.log(args[0]);
            },
            onLeave: function(retval){
				//if (retval != 0)
                //console.log("onLeave---------------------------------");
				//console.log(Thread.backtrace(this.context, Backtracer.ACCURATE).map(DebugSymbol.fromAddress).join('\\n') + '\\n');
				//console.log(retval);
				console.log("onLeave sub_13EF3C--------------------------------->\\n");
            }
        });
     }*/
		
	//0x794174 time
	//0x795DEC	sleep	loop时没有调用sleep相关的函数
	//
	//794BBC
	//79834C
	//793F48
	 var funAddr1 = soAddr.add(0x793F48);
	 console.log(funAddr1);
	 if(funAddr1 != null){
	 console.log("attach");
        Interceptor.attach(funAddr1,{
            onEnter: function(args){
				console.log("onEnter---------------------------------<");
				console.log(Thread.backtrace(this.context, Backtracer.ACCURATE).map(DebugSymbol.fromAddress).join('\\n') + '\\n');
            },
            onLeave: function(retval){
				console.log("onLeave--------------------------------->\\n");
				console.log(Thread.backtrace(this.context, Backtracer.ACCURATE).map(DebugSymbol.fromAddress).join('\\n') + '\\n');
            }
        });
     }
	 
	 /*
Address	Function	Instruction
.text:00109474	sub_109114	SUB     R2, R11, #-var_108	no loop !_ZNSt11__copy_move
.text:00148430	sub_148410	ADD     R3, R3, #0x108	no libpvz.so!toupper+0x498ac Java_com_popcap_pvz_1na_XPromoDynamicButtonView_NativeSetDefaultMoreGamesButtonVisible+0x10468
.text:001486F0	sub_1486C8	ADD     R2, R3, #0x108 no 选择要种的植物时调用
.text:0014D7C8	sub_14C340	ADD     R2, R3, #0x108 no 选择要种的植物时调用
.text:00165D84	sub_165D1C	ADD     R3, R3, #0x108 no loop
.text:001677DC	sub_1677C0	ADD     R3, R3, #0x108 no loop
######.text:001943E4	sub_193614	ADD     R3, R3, #0x108 maybe		收阳光 金钱时会调用 sub_195628 sub_33456C sub_1DACB4 sub_1DAC78
.text:001AF580	sub_1AF4B8	LDRB    R3, [R3,#0x108] no loop
.text:001F4D28	sub_1F46E0	LDR     R3, [R3,#0x108] no loop Java_com_popcap_pvz_1na_PvZActivity_onPVZXpromoCreate
.text:0021D8D8	sub_21D88C	LDR     R2, [R3,#0x108] no loop !_ZNSt11__copy_move
.text:0021E020	sub_21E000	LDR     R3, [R3,#0x108] no loop !_ZNSt11__copy_move
	 */
	 
	 
	 //sub_1659B0 阳光
	 /*var funAddr3 = soAddr.add(0x001659B0);
	 console.log(funAddr3);
	 if(funAddr3 != null){
	 console.log("attach sub_1659B0");
        Interceptor.attach(funAddr3,{
            onEnter: function(args){
				//console.log("onEnter sub_1659B0---------------------------------<");
            },
            onLeave: function(retval){
				//console.log("onLeave sub_1659B0--------------------------------->\\n");
            }
        });
     }*/
}

function hookTest32(){
    var helloAddr = Module.findExportByName("libCore.so", "malloc");
    console.log("libCore.so malloc addr:	" + helloAddr);
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