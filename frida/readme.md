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