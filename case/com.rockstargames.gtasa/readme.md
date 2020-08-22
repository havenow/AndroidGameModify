#

```
sadls0122g 低版本的 只能在模拟器中运行 带修改器
gtasa 新版本 在安卓10上无法运行 不带修改器

带修改器的apk里面有libcleo.so，应该是cheat menu的库
libGTASA.so里面有一个cheat类，GetPad来处理cheat

侠盗猎车手:圣安地列斯 安卓版 调出作弊码方法教程		试过好像不行
https://www.jb51.net/gonglue/124541.html


```

#
```
分析gg大玩家带cheat菜单的包：
1、提取侠盗猎车手的盒子包，assets目录下面plugin-gta.apk是cheat菜单，nativeDoCheat(int i, int i2, int i3)是cheat开关的函数
参数i是Nativeid，i2是type, i3是开关，Nativeid和type存在Function类的list中
2、分析plugin-gta.apk中的libpgod.so，JNI_OnLoad/sub_1D8C/sub_22FC 会dlsym下面的函数：
_ZN6CCheat11HealthCheatEv ... 其他CCheat类的函数
"_ZN6CTimer6UpdateEv"
"_Z20SCSetCurrentLangaugePKc"
"_Z16AND_DeviceLocalev"

3、分析libpgod.so的jniDoProcessCheat函数
根据前面Nativeid和type做switch case
分析其中一个cheat，CCheat::SuicideCheat 问题：有四个参数，新版的是不带参数的，老版的游戏apk提取失败

          case 8:
            result = dword_6084(1, 2057270476, 1074733655, 23);
            break;

.data.rel.ro.local:00005CD4                 DCD dword_6084
.data.rel.ro.local:00005CD8                 DCD aZn6ccheat12sui     ; "_ZN6CCheat12SuicideCheatEv"


.bss:00006084 ; int (__fastcall *dword_6084)(_DWORD, _DWORD, _DWORD, _DWORD)
.bss:00006084 dword_6084      % 4                     ; DATA XREF: sub_1974+218↑r
.bss:00006084                                         ; .data.rel.ro.local:00005CD4↑o

```

# 破解思路
```
首先是对游戏的注入，写的代码编译成so，注入到游戏进程
如果直接讲写的so打进游戏包里面，在java层loadLibrary，就不用注入so了
在GTASA.smali中添加
    const-string v0, "inlineHook"
    invoke-static {v0}, Ljava/lang/System;->loadLibrary(Ljava/lang/String;)V
	
对libGTASA.so中的函数进行inline hook，可以实现replace函数

下面是关键点：！！！！！！！！！！！！！！！！！！！！！！！！
查看libCEO.so
signed int __fastcall JNI_OnLoad(int a1)
int sub_51BC()
int __fastcall sub_46DC(int a1, int a2)
函数中会dlsym libGTASA.so中的函数，查看了下
感觉_ZN14CRunningScript17ProcessOneCommandEv有点像使能cheat的函数

hook replace libGTASA.so中的ProcessOneCommand函数：	hook replace都成功了，但是游戏会闪退
CRunningScript::ProcessOneCommand
继续查看调用
int __fastcall CRunningScript::Process(CRunningScript *this)
int __fastcall CGame::Process(CGame *this)

感觉CGame::Process比较像游戏跑每一帧的函数
hook replace 函数CGame::Process，在replace函数中打印日志，发现进入游戏场景后，会不停的输出
int __fastcall CGame::Process(CGame *this)


在函数CGame::Process的replace函数中，调用f__ZN6CCheat12WeaponCheat1Ev，WeaponCheat1是生效的

如果直接调用WeaponCheat1游戏是会崩溃的，调用应该是在游戏的帧与帧之间
```