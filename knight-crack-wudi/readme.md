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














