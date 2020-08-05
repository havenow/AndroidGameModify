# 安装

```
https://github.com/frida/frida
https://github.com/frida/frida/releases

https://frida.re/

https://blog.csdn.net/hujun5281/article/details/106516970/
https://blog.csdn.net/guo343310267/article/details/88343025


pip install frida-tools
第一次运行次命令有一个报错
在运行一次安装成功了
```

```
adb forward tcp:27042 tcp:27042
adb forward tcp:27043 tcp:27043

demo
https://www.bilibili.com/video/BV1jJ411K7Xx/?spm_id_from=333.788.videocard.7
https://github.com/abcdsxg/frida-practice/tree/master/practice-1
用eclipse写的工程hook失败了，Android Studio的工程hook成功
public class MainActivity extends AppCompatActivity {		Android Studio

public class MainActivity extends ActionBarActivity {		eclipse
```

```
Java层Hook(普通方法、构造方法、重载方法、构造对象参数、修改对象属性)
https://www.bilibili.com/video/BV1Z4411j7at/?p=3
https://www.bilibili.com/video/BV1Ei4y1b7ST	打印Java层函数堆栈定位关键代码

YouTube教程 Android逆向---Hook神器家族的Frida工具使用详解
https://www.bilibili.com/video/BV1yE411f7uW/?spm_id_from=333.788.videocard.11
```

```
Frida hook native中的函数
https://www.jianshu.com/p/b833fba1bffe

还可以按照wp Level1 Writeup使用Frida hook代码获取flag。

Frida使用的过程中不能和Magisk Hide一起使用, 不然会出现Failed to spawn: unable to access zygote64 while preparing for app launch; try disabling Magisk Hide in case it is active的报错。所以我们需要先关掉Magisk Hide(Magisk Manager > Settings >Magisk > Magisk Hide (关掉)), 并且还要在Frida脚本里面绕过root的检测。

ipython
https://blog.csdn.net/qq_39362996/article/details/82892671
```

```
frida objection ZenTracer
https://www.anquanke.com/post/id/197657

objection的安装：
pip3 install objection

objection注入“设置”应用
	objection -g com.android.settings explore
查看内存中加载的库
	memory list modules
查看库的导出函数
	memory list exports libssl.so
将结果保存到json文件中
	 memory list exports libart.so --json c:\\libart.json 

好像没法hook so??
```

```
FRIDA系列文章
https://github.com/r0ysue/AndroidSecurityStudy
```

```
frida doc
https://frida.re/docs/

frida好像无法attach /data/local/tmp下面启动的elf程序
  printf("pid: %d\n", getpid());
  printf ("f() is at %p\n", f);
  session = frida.attach(30434)
根据pid 去attach会失败，adb shell ps > msg.txt 查看不到进程号

Interceptor NativePointer(Function/Callback)使用
https://www.anquanke.com/post/id/195869#h3-16
```

```
使用frida框架hook so的例子，用来打印函数参数，返回值，修改返回值
hook的是chrom浏览器 com.android.chrome
启动时 open.py pid

import frida
import sys
import io

device = frida.get_usb_device()

session = device.attach(int(sys.argv[1]))

scr = """
setImmediate(function() {
Interceptor.attach(Module.findExportByName("libc.so" , "open"), {
    onEnter: function(args) {

        send("open called! args[0]:",Memory.readByteArray(args[0],256));		将libc.so中open函数的第一个参数存到文件D:\\log.txt中
		console.log('args[1]  : ' + args[1].toInt32());							打印libc.so中open函数的第二个参数
    },
    onLeave:function(retval){
		console.log('retval  :' + retval.toInt32());
		retval.replace(666);													修改libc.so中open函数的返回值
		console.log('retval replace :' + retval.toInt32());						注意这里的修改返回值有可能导致attach的进程崩溃
    }
});

});
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
```

```
使用frida框架hook so的例子，用来replace函数
hook的是chrom浏览器 com.android.chrome
启动时 openReplace.py pid

import frida
import sys
import io


device = frida.get_usb_device()

session = device.attach(int(sys.argv[1]))

scr = """
setImmediate(function() {
	var open_method = new NativeFunction(Module.findExportByName('libc.so', 'open'), 'int',['pointer','int']);
	 Interceptor.replace(open_method, new NativeCallback(function (a, b) {
            return -1;															hook了libc.so中open函数，直接返回-1
       }, 'int', ['pointer', 'int']));
});
"""

def on_message(message ,data):
   print('on_message')


script = session.create_script(scr)
script.on("message" , on_message)
script.load()
sys.stdin.read()


先openReplace.py pid
在open.py pid
会看到调用open函数之后的的返回值都是-1
```

