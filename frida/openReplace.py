import frida
import sys
import io


device = frida.get_usb_device()

session = device.attach(int(sys.argv[1]))

scr = """
setImmediate(function() {
	var open_method = new NativeFunction(Module.findExportByName('libc.so', 'open'), 'int',['pointer','int']);
	 Interceptor.replace(open_method, new NativeCallback(function (a, b) {
            return -1;															
       }, 'int', ['pointer', 'int']));
});
"""

def on_message(message ,data):
   print('on_message')


script = session.create_script(scr)
script.on("message" , on_message)
script.load()
sys.stdin.read()