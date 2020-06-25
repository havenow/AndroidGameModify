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
结合IDA和dnSpy来定位变量或者函数来达到修改的目的       

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