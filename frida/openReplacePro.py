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

def on_message(message ,data):
   print('on_message')


script = session.create_script(scr)
script.on("message" , on_message)
script.load()
sys.stdin.read()