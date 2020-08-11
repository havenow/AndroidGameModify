import frida
import sys
import io


device = frida.get_usb_device()

session = device.attach(int(sys.argv[1]))

scr = """
function hookTest1(){
	var imports = Module.enumerateImports("libhookDemo.so");
	
	console.log('test');
	for(var i = 0; i < imports.length; i++){
		//if(imports[i].name == "strncat"){
			console.log(JSON.stringify(imports[i]));
			console.log(imports[i].address);
		//}
	}
}

function hookTest2(){
	var exports = Module.enumerateExports("libhookDemo.so");
	for(var i = 0; i < exports.length; i++){
		//if(exports[i].name == "strncat"){//当不确定导出表里面的全称，用部分关键字查找时使用
			console.log(JSON.stringify(exports[i]));
		//}
	}
}

function hookTest3(){
	var addr = Module.findExportByName("libhookDemo.so", "_ZN6CCheat12WeaponCheat1Ev");
	console.log(addr);
}

function hookTest4(){
	var symbols = Module.enumerateSymbols("libhookDemo.so");
	if (symbols == null)
		console.log("symbols == null");
	else
		console.log("symbols != null");
	console.log(symbols.length);
	for(var i = 0; i < symbols.length; i++){
		console.log(JSON.stringify(symbols[i]));
	}
}


function main(){
	//hookTest1();
	hookTest2();
	//hookTest3();
	hookTest4();
}

setImmediate(main);
"""

def on_message(message ,data):
   print('on_message')


script = session.create_script(scr)
script.on("message" , on_message)
script.load()
sys.stdin.read()