import frida
import sys
import io


device = frida.get_usb_device()

session = device.attach(int(sys.argv[1]))

scr = """
setImmediate(function() {
	var open_ptr = Module.findExportByName('libc.so', 'open');
	var open_method = new NativeFunction(open_ptr, 'int',['pointer','int']);
	 Interceptor.replace(open_method, new NativeCallback(function (pathPtr, flags) {
	 
			var path = pathPtr.readUtf8String();
			console.log('args[0]  : ' + path);
			console.log('args[1]  : ' + flags);
			
			var fd = open_method(pathPtr, flags);	
			
			console.log('retval  : ' + fd);
		
            return fd;
       }, 'int', ['pointer', 'int']));
});
"""

/*
function hookTest11(){
    Java.perform(function(){
        var funcAddr = Module.findBaseAddress("libhookDemo.so").add(0x23F4);
        var func = new NativeFunction(funcAddr, "pointer", ['pointer', 'pointer']);
        var env = Java.vm.tryGetEnv();
        console.log("env: ", JSON.stringify(env));
        if(env != null){
            var jstr = env.newStringUtf("hook test!!!");
            //console.log("jstr: ", hexdump(jstr));
            var cstr = func(env, jstr);
            console.log(cstr.readCString());
            console.log(hexdump(cstr));
        }
    });
}
*/

def on_message(message ,data):
   print('on_message')


script = session.create_script(scr)
script.on("message" , on_message)
script.load()
sys.stdin.read()