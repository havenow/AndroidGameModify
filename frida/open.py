import frida
import sys
import io

device = frida.get_usb_device()

session = device.attach(int(sys.argv[1]))

scr = """
setImmediate(
function() 
{
Interceptor.attach(
Module.findExportByName("libc.so" , "open"), 
{
    onEnter: function(args) 
	{

        send("open called! args[0]:",Memory.readByteArray(args[0],256));
		console.log('args[1]  : ' + args[1].toInt32());
    },
	
    onLeave:function(retval)
	{
		console.log('retval  :' + retval.toInt32());
    }
}
);

}
);
"""

def on_message(message ,data):
    file_object=open("D:\\log.txt",'ab+')
    file_object.write(message['payload'].encode())
    file_object.write(data.split(b'\x00')[0])
    file_object.write('\n'.encode())
    file_object.close()


script = session.create_script(scr)
script.on("message" , on_message)
script.load()
sys.stdin.read()