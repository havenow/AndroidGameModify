import frida
import sys
import io

device = frida.get_usb_device()

session = device.attach("com.popcap.pvz")

scr = """
/*
两个比较有价值的链接：
https://www.52pojie.cn/thread-852981-1-1.html(android调试：敲牛逼的frida弥补arm架构下无法设置硬件读写断点的缺陷)
https://github.com/binge1993/frida-snippets#table-of-contents(One time watchpoint)

加内存断点调试要改aosp内核?	Android Open Source Project Android系统源码项目
只能设置1k对齐的 大小为1024的整数倍，难道需要改安卓源码？楼主能提供具体的安卓版本和frida版本吗
Memory.protect(ptr('0xB582CF44'), 4, 'rw-');	frida的Memory.protect()函数第二个参数的含义是什么，是4*1024???
如果大小只能是1024的整数倍，设置内存断点就会有问题：不能精准定位内存的读写操作
//0x9D5FBC84 太阳	定位到的是0x9D5FBC74，而且这个地址会高频率的被写
//0xB582CF44 金币	定位到的是0xb582cd6c，而且这个地址会高频率的被写

打印信息 e.g. 打印堆栈，打印发生异常的地址，打印引发异常的地址
                        type: string specifying one of:
                                abort
                                access-violation
                                guard-page
                                illegal-instruction
                                stack-overflow
                                arithmetic
                                breakpoint
                                single-step
                                system
                        address: address where the exception occurred, as a NativePointer(发生异常的地址：具体函数调用的行)
                        memory: if present, is an object containing:
                                operation: the kind of operation that triggered the exception, as a string specifying either read,  write, or execute
                                address: address that was accessed when the exception occurred, as a NativePointer（引发异常的地址，比如：改写了血对应的地址）
                        context: object with the keys pc and sp, which are NativePointer objects specifying EIP/RIP/PC and ESP/RSP/SP, respectively, for ia32/x64/arm. Other processor-specific keys are also available, e.g. eax, rax, r0, x0, etc. You may also update register values by assigning to these keys.
                        nativeContext: address of the OS and architecture-specific CPU context struct, as a NativePointer. This is only exposed as a last resort for edge-cases where context isn’t providing enough details. We would however discourage using this and rather submit a pull-request to add the missing bits needed for your use-case.               
    
	
	console.log(exp.address);
	console.log(exp.memory.address);	

处理异常
		Memory.protect(ptr('0xB582CF44'), 4, 'rw-');	让内存0xB582CF44可读可写，这里指定的4字节好像不起作用？

*/

function set_read_write_break()
{
	var soAddr = Module.findBaseAddress("libpvz.so");
    //console.log(soAddr);
	console.log("set_read_write_break---------------------------------begin");
	Process.setExceptionHandler(function(exp) {//当写内存的时候进入Handler，打印相关信息之后，将内存属性改为可读可写
		console.log("ExceptionHandler begin");
		//console.log('address that was accessed:            ' + exp.memory.address);//实际发生写操作的内存地址
		//console.log('address where the exception occurred: ' + exp.address);//发生写内存时，哪一行代码做了写操作
		if (exp.memory.address == 0xC1A08144){
		//if (true){
			console.log('address libpvz.so base:               ' + soAddr);//app运行的时候，libpvz.so在系统中的基地址
			console.log('address where the exception occurred: ' + exp.address);//发生写内存时，哪一行代码做了写操作
			console.log('address where called in libpvz.so   : ' + ptr(exp.address - soAddr));//根据这个地址偏移，可以在IDA中跳转到libpvz.so对应的行
			console.log('address that was accessed:            ' + exp.memory.address);//实际发生写操作的内存地址(有一个问题：和Memory.protect指定的内存地址不一样)
		
			console.warn(JSON.stringify(Object.assign(exp, { _lr: DebugSymbol.fromAddress(exp.context.lr), _pc: DebugSymbol.fromAddress(exp.context.pc) }), null, 2));
		}
		else
			;//console.log('address that was accessed:            ' + exp.memory.address);
		
		//Memory.protect(exp.memory.address, Process.pointerSize, 'rw-');
		Memory.protect(ptr('0xC1A08144'), 4, 'rw-');
		// can also use `new NativeFunction(Module.findExportByName(null, 'mprotect'), 'int', ['pointer', 'uint', 'int'])(parseInt(this.context.x2), 2, 0)`
		console.log("ExceptionHandler end");
		return true; // goto PC 
	});
	//制造异常 <--> 设置读写断点		rx:设置一个写断点	wx:设置一个4字节的读断点	rw:设置一个执行断点,可以代替F2软断点，可以过断点crc校验
    //Memory.protect(addr, size, pattern);
	/*console.log('set one time watchpoint', JSON.stringify({
      mprotect_ret: Memory.protect(ptr('0xA8C9730C'), 4, 'r-x'),
    }, null, 2));*/
	 Memory.protect(ptr('0xC1A08144'), 4, 'r--');//注意这里是r--，而不是r-x，因为这个内存块的属性是可读可写，没有可执行
	//0x9D5FBC84 太阳	定位到的是0x9D5FBC74
	//0xB582CF44 金币	定位到的是0xb582cd64 0xb582cd6c
	//Memory.protect(addr, size, 'r-x');
	/*
		这里如果是'rw'，不会进入setExceptionHandler设置的回调中
		update protection on a region of kernel memory
		addr地址更新成可读可执行，如果写这个地址时，会产生异常，进入setExceptionHandler设置的回调中
		回调里面Memory.protect(exp.memory.address, 4, 'rw-'); 改回内存是可读读写，后续就不会一直进入回调，
		一直进入回调，app会崩溃，不知道原因
	*/
	console.log("set_read_write_break---------------------------------end");
}

function sleep(numberMillis) {  
    var now = new Date();  
    var exitTime = now.getTime() + numberMillis;  
    while (true) {  
        now = new Date();  
        if (now.getTime() > exitTime)  
        return;  
        }  
}

function main(){
	console.log("main---------------------------------begin");

	//One time watchpoint	只能使用一次？
	set_read_write_break();
	/*for (var i=0; i<2; i++)
	{
      set_read_write_break();
	  sleep(1000);
	}*/
	/*do{
		set_read_write_break();
		sleep(10000);
	}while(1)*/
	
	console.log("main---------------------------------end");
}

setImmediate(main);
/*for (var i=0; i < 100000000; i++)
{
  setImmediate(main);
  sleep(100);
}*/
/*do{
  setImmediate(main);
  sleep(100);
}while(1)*/
//setImmediate(main);
//setImmediate(main);
"""

def on_message(message ,data):
   print('on_message')


script = session.create_script(scr)
script.on("message" , on_message)
script.load()
sys.stdin.read()