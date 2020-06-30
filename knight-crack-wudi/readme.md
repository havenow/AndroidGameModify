# 关键代码
```
bx lr		1E FF 2F E1
mov r0,#0	00 00 A0 E3
mov r0,#1	01 00 A0 E3
nop			00 00 A0 E1

dllBaseAddr 是通过/proc/uid/maps文件获取
if (sscanf(line, "%lx-%lx %c%c%c%c %x %x:%x %u %[^\n]", &start, &end, &read,
                &write, &exec, &cow, &offset, &dev_major, &dev_minor, &inode, filename) >= 6) 
		{
			char* str = NULL;
			str = strstr(filename, "libil2cpp.so");
			//fprintf(stderr, "find %x ~ %x; %s\n", start, end, filename);
			if (str)
			{
				 fprintf(stderr, "---find %x ~ %x; %s\n", start, end, filename);
				 dllBaseAddr = start;
				 //dllBaseAddr = (void *)(start + 0x70A5F0);
				 return true;
			}
           
        }
		

修改关键函数对应的机器码，写入八字节的机器码

char ori_buf[]= {0x18, 0xB0, 0x8D, 0xE2, 0x08, 0xD0, 0x4D, 0xE2};
char buf[] = { 0x00, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1 };//两条指令：mov r0,#0 和 bx lr
void *data;
data = buf;

ptrace(PTRACE_POKEDATA, target_pid, (void *)(dllBaseAddr + 0x70A5F0 + 4), *(long *)(data + 0));//在地址0xBA4195F4写入机器码0x00 0x00 0xA0 0xE3
ptrace(PTRACE_POKEDATA, target_pid, (void *)(dllBaseAddr + 0x70A5F0 + 4+ 4), *(long *)(data + 4));//在地址0xBA4195F8写入机器码0x1E 0xFF 0x2F 0xE1

```
# 关键函数怎么定位
部分可以通过GG来搜索定位关键点在内存中的实际地址，结合libil2cpp.so的起始地址，算出偏移       
结合IDA和dnSpy来定位变量或者函数来达到修改的目的（变量只有相对this的偏移）       

```
在dnSpy中搜索到了
		// Token: 0x040029A9 RID: 10665
		[FieldOffset(Offset = "0x4C")]
		public int coin

用GG搜索金币的内存地址
搜索三次
B3272FD4
B3272FD4 - b3266000 = CFD4
info: ### 120, size: 626688, name: , b3266000-b32ff000, type: 0

C1EEC944
C1EEC944 - c1ede000 = E944 		
info: ### 211, size: 626688, name: , c1ede000-c1f77000, type: 0

C30D397C
C30D397C - c30c4000 = F97C
info: ### 234, size: 626688, name: , c30c4000-c315d000, type: 0

变量的地址，好像没法办法重新定位
在size: 626688这个内存块里面，不在libil2cpp.so的内存块里面，而且每次相对内存块的相对偏移也不一样
```

# STR LDR指令

```
STR{条件}  源寄存器，<存储器地址>
STR指令用亍从源寄存器中将一个32位的字数据传送到存储器中。

STR R0，[R1]，＃8             ；将R0中的字数据写入以R1为地址的存储器中，并将新地址R1＋8写入R1。
STR R0，[R1，＃8]             ；将R0中的字数据写入以R1＋8为地址的存储器中。”
STR     r1, [r0]              ；将r1寄存器的值，传送到地址值为r0的（存储器）内存中


LDR 从存储器中加载数据到寄存器
LDR R3,=0X40000000;LDR伪指令，把地址加载到寄存器里面去
LDR R4,[R3,#0x04];把首地址加上偏移量得到数据的地址再寻址 R3------->0x40000000+4=0x40000004----->4
ldr r0, =0x12345678
这样，就把0x12345678这个地址写到r0中了。所以，ldr伪指令和mov是比较相似的。只不过mov指令限制了立即数的长度为8位，也就是不能超过512。而ldr伪指令没有这个限制。如果使用ldr伪指令时，后面跟的立即数没有超
```

# arm汇编函数调用参数传递规则	

```
arm汇编函数调用参数传递规则	
	 1. 子程序通过寄存器R0~R3来传递参数. 这时寄存器可以记作: A0~A3 , 被调用的子程序在返回前无需恢复寄存器R0~R3的内容.
     2. 在子程序中,使用R4~R11来保存局部变量,这时寄存器R4~R11可以记作: V1~V8 .如果在子程序中使用到V1~V8的某些寄存器,子程序进入时必须保存这些寄存器的值,在返回前必须恢复这些寄存器的值,对于子程序中没有用到的寄存器则不必执行这些操作.在THUMB程序中，通常只能使用寄存器R4~R7来保存局部变量.
     3.寄存器R12用作子程序间scratch寄存器,记作ip; 在子程序的连接代码段中经常会有这种使用规则.
     4. 寄存器R13用作数据栈指针,记做SP,在子程序中寄存器R13不能用做其他用途. 寄存器SP在进入子程序时的值和退出子程序时的值必须相等.
     5. 寄存器R14用作连接寄存器,记作lr ; 它用于保存子程序的返回地址,如果在子程序中保存了返回地址,则R14可用作其它的用途.
     6. 寄存器R15是程序计数器,记作PC ; 它不能用作其他用途.
     7. ATPCS中的各寄存器在ARM编译器和汇编器中都是预定义的.	
```

# IDA动态调试so问题
https://www.bilibili.com/video/BV1ub411E7wS?from=search&seid=15387634077601357608      
```
触发断点后，游戏直接闪退了

尝试用下面的方法
	./data/local/tmp/android_server
	adb forward tcp:23946 tcp:23946	
	adb shell am start -D -n com.knight.union.aligames/com.chillyroom.plugin.wrapper4399.ActivityWrapper4399
	IDA附加进程
	C:\Java\jdk1.8.0_31\bin\jdb -connect com.sun.jdi.SocketAttach:hostname=localhost,port=8700
	
DDMS无法看到进程
临时解决方案 添加 <application android:debuggable="true" 重新打包
修改ro.debuggable为1 尝试用mprop修改失败	

jdb -connect com.sun.jdi.SocketAttach:hostname=localhost,port=8700是提示
致命错误:
无法附加到目标 VM
```

# 使用老手机IDA动态调试so

```
./data/local/tmp/android_server
adb forward tcp:23946 tcp:23946	
直接手机上启动游戏，玩到场景1
IDA附加进程

调试时可以进入断点

如果使用C:\Java\jdk1.8.0_31\bin\jdb -connect com.sun.jdi.SocketAttach:hostname=localhost,port=8700命令
还是会提示
致命错误:
无法附加到目标 VM
```