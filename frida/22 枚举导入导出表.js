
//frida -U me.shingle.fridatest -l E:\mod\1\AndroidGameModify\frida\hook.js	
//下面这样先启动，在load js，看起来没成功
//frida -U -f me.shingle.fridatest --no-pause
//%load E:\mod\1\AndroidGameModify\frida\hook.js
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
		//if(exports[i].name == "strncat"){
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
	hookTest1();
	//hookTest2();
	//hookTest3();
	//hookTest4();
}

setImmediate(main);