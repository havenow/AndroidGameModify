import frida
import sys
import io


device = frida.get_usb_device()

session = device.attach("com.popcap.pvz_na")

scr = """
function hookTest(){
    var soAddr = Module.findBaseAddress("libpvz.so");
    console.log(soAddr);

	//clock_gettime	.plt	00051C2C	0000000C			R	.	.	.	.	T	.
	//gettimeofday	.plt	00051AD0	0000000C			R	.	.	.	.	T	.
	//gmtime	.plt	00051974	0000000C			R	.	.	.	.	T	.
	//localtime	.plt	00051950	0000000C			R	.	.	.	.	T	.
	//localtime_r	.plt	00051968	0000000C			R	.	.	.	.	T	.
	//mktime	.plt	00051A94	0000000C			R	.	.	.	.	T	.
	//time	.plt	00051908	0000000C			R	.	.	.	.	T	.
	//utime	.plt	00051D04	0000000C			R	.	.	.	.	T	.
	
	/*
Address	Ordinal	Name	Library
0119C9C8		time	
0119C9E4		localtime	
0119C9EC		localtime_r	
0119C9F0		gmtime	
0119CA6C		clock_gettime	
0119CAB8		gettimeofday	
0119CAD0		mktime	
0119CB1C		pthread_cond_timedwait	
0119CBA8		sem_timedwait	
0119CC00		utime	
0119CC0C		timezone	

	*/
	//sub_794174

    var funcAddr = soAddr.add(0x794174); //函数地址计算 thumb+1 ARM不加	
    console.log(funcAddr);
	
	var count = 0;

    if(funcAddr != null){
		console.log("attach");
        Interceptor.attach(funcAddr,{
		
		
            onEnter: function(args){
				//console.log("onEnter ---------------------------------<");
				//console.log(Thread.backtrace(this.context, Backtracer.ACCURATE).map(DebugSymbol.fromAddress).join('\\n') + '\\n');
				//console.log(args[0]);
            },
            onLeave: function(retval){
				count = count +1;
				//console.log(retval.toInt32()*count);
				retval.replace(retval.toInt32()*count*100);	
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