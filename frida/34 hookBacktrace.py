import frida
import sys
import io


device = frida.get_usb_device()

session = device.attach(int(sys.argv[1]))

scr = """
function hookTest(){
	var addr = Module.findExportByName("libhookDemo.so", "_ZN6CCheat12WeaponCheat1Ev");
	console.log(addr);
	Interceptor.attach(addr, {
		onEnter: function (args){
			console.log('libhookDemo called from:\\n' +
				Thread.backtrace(this.context, Backtracer.ACCURATE)
				.map(DebugSymbol.fromAddress).join('\\n') + '\\n');
		}
	});
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