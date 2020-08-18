import frida
import sys
import io


device = frida.get_usb_device()

session = device.attach(int(sys.argv[1]))

scr = """

function stringToBytes(str) {  
    var ch, st, re = []; 
    for (var i = 0; i < str.length; i++ ) { 
        ch = str.charCodeAt(i);
        st = [];                 
        do {  
            st.push( ch & 0xFF );  
            ch = ch >> 8;          
        }    
        while ( ch );  
		re = re.concat( st.reverse() );
    }  
    return re;  
}

function hookTest(){
	var soAddr = Module.findBaseAddress("libhookDemo.so");
	console.log(soAddr);
	if (soAddr != null){
		//console.log(soAddr.add(0x2D4A).readCString());
		//console.log(hexdump(soAddr.add(0x2D5A)));//读取指定地址的字符串
		
		//var strByte = soAddr.add(0x2D4A).readByteArray(16);//读内存 16字节
		//console.log(strByte);
		
		//var writeBytes = stringToBytes("wangfeng");
		//console.log(writeBytes);
		//soAddr.add(0x2D4A).writeByteArray(writeBytes);	//此处应用程序崩溃了 是因为这个String 在rodata区？？？
		//console.log(hexdump(soAddr.add(0x2D4A)));//dump指定内存
		
		//var bytes = Memory.readByteArray(soAddr.add(0x2D4A), 16); //原先API
        //console.log(bytes);
		
		var arr = [ 0x72, 0x6F, 0x79, 0x73, 0x75, 0x65];
		const r = Memory.alloc(arr.length);
		Memory.writeByteArray(r,arr);
		var buffer = Memory.readByteArray(r, arr.length);
		/*console.log(hexdump(buffer, {
            offset: 0,
            length: arr.length,
            header: true,
            ansi: false
        }));*/
		console.log(hexdump(r));
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