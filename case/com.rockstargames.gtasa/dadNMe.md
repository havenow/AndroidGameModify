#
```
游戏开局的是会malloc 一块内存，malloc两次，里面有血和狂暴

```

```
64的游戏程序 ：
hook malloc，分配0x1408大小的内存块
血的偏移 =   0x130C;
狂暴的偏移 = 0xD44;

游戏启动
---begin
0x1408
0x70b0b00800			(blood crazy)00000000 00000000
---end


---begin
0x1408
0x70b0b64800			(blood crazy)00000000 ff3300ff(-13434625)
---end


被打死后， 第二次继续
---begin
0x1408
0x710a84a000			(blood crazy)0000002e 00000104
---end


---begin
0x1408
0x710a84a000			(blood crazy)00000000 00000104
---end


被打死后， 第三继续
---begin
0x1408
0x70bb56f000			(blood crazy)00000000 00000000
---end


---begin
0x1408
0x70bb56f000			(blood crazy)00000000 00000000
---end

第一次分配的两个0x1408 地址不一样
第二次分配的两个0x1408 地址一样
第三次分配的两个0x1408 地址一样

直接每次更新地址
```

```
32位游戏程序：
hook malloc，分配0x1008大小的内存块
血的偏移 =   0xF44;
狂暴的偏移 = 0xAA4;


2020-09-16 19:20:55.156 26938-26938/? E/inlinehook: alternatvie DadNMe _bloodAddr _crazyAddr: ccb71f44 ccb71aa4 14198 1868784742
2020-09-16 19:20:55.572 26938-26989/? E/inlinehook: alternatvie DadNMe _bloodAddr _crazyAddr: b3bc6b44 b3bc66a4 0 0
2020-09-16 19:20:55.572 26938-26989/? E/inlinehook: alternatvie DadNMe _bloodAddr _crazyAddr: b1e0bf44 b1e0baa4 0 0
2020-09-16 19:20:55.574 26938-26989/? E/inlinehook: alternatvie DadNMe _bloodAddr _crazyAddr: b1e0e744 b1e0e2a4 0 0
2020-09-16 19:20:55.582 26938-26989/? E/inlinehook: alternatvie DadNMe _bloodAddr _crazyAddr: b1e0bf44 b1e0baa4 -1307031592 -1307032616
2020-09-16 19:20:55.594 26938-26989/? E/inlinehook: alternatvie DadNMe _bloodAddr _crazyAddr: b1e0e744 b1e0e2a4 -1310341600 0
2020-09-16 19:21:06.218 26938-26989/? E/inlinehook: alternatvie DadNMe _bloodAddr _crazyAddr: af0a4f44 af0a4aa4 1065353216 1			(important!!!!)		3F800000 00000001
2020-09-16 19:21:13.305 26938-26989/? E/inlinehook: +++update DadNMe _bloodAddr _crazyAddr: af0a4f44 af0a4aa4 1065353216 1146129076							3F800000 44508AB4
2020-09-16 19:21:25.418 26938-27041/? E/inlinehook: alternatvie DadNMe _bloodAddr _crazyAddr: b3451b44 b34516a4 0 0						(此句可能插在中间)	00000000 00000000

2020-09-16 20:01:56.404 26938-26989/? E/inlinehook: alternatvie DadNMe _bloodAddr _crazyAddr: b4156744 b41562a4 0 0						(important!!!!)
2020-09-16 20:02:03.395 26938-26989/? E/inlinehook: +++update DadNMe _bloodAddr _crazyAddr: b4156744 b41562a4 0 0

2020-09-16 20:03:38.065 26938-26989/? E/inlinehook: +++update DadNMe _bloodAddr _crazyAddr: b4156744 b41562a4 0 0						(important!!!!)
2020-09-16 20:03:45.125 26938-26989/? E/inlinehook: +++update DadNMe _bloodAddr _crazyAddr: b4156744 b41562a4 0 0

2020-09-16 20:05:03.624 26938-26989/? E/inlinehook: +++update DadNMe _bloodAddr _crazyAddr: b4156744 b41562a4 0 0						(important!!!!)
2020-09-16 20:05:10.672 26938-26989/? E/inlinehook: +++update DadNMe _bloodAddr _crazyAddr: b4156744 b41562a4 0 0

2020-09-16 20:13:05.454 26938-26989/? E/inlinehook: alternatvie DadNMe _bloodAddr _crazyAddr: b4153f44 b4153aa4 0 0						(important!!!!)
2020-09-16 20:13:12.568 26938-26989/? E/inlinehook: +++update DadNMe _bloodAddr _crazyAddr: b4153f44 b4153aa4 0 0

_bloodAddr _crazyAddr:的值等于 1065353216 1146129076，更新地址 （解决  此句插在中间了）
连续两次地址相同，更新地址

注意大小端，这里是小端，小端：低位数据存放在低位地址
从frida中打印的数据来看
---begin
0x1008
0xa5d32000
---blood value
           0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F  0123456789ABCDEF
00000000  00 00 80 3f(打印内存中的数据: 低位数据->高位数据)...?					0x3f800000	写代码时，数据的表示：0x高位数据...低位数据
---crazy value
           0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F  0123456789ABCDEF
00000000  b4 8a 50 44                                      ..PD					0x448a5044
---end
```

```
32的游戏对应的地址判断
		if (0x1008 == size)
		{
			if (ret != _alternativeBaseAddr)
			{
				_alternativeBaseAddr = ret;
				LOGE("alternatvie DadNMe _bloodAddr _crazyAddr: %x %x %d %d\n", ret + nDeltaBlood32, ret + nDeltaCrazy32, 
				*(static_cast<unsigned int*>(ret + nDeltaBlood32)), *(static_cast<unsigned int*>(ret + nDeltaCrazy32)));
				unsigned int* addrTmp1 = static_cast<unsigned int*>(ret + nDeltaBlood32);
				unsigned int* addrTmp2 = static_cast<unsigned int*>(ret + nDeltaCrazy32);
				//LOGE("value: %d %d\n", (int)(*addrTmp1), (int)(*addrTmp2));
				if ((1065353216 == (*addrTmp1)) && (1146129076 == (*addrTmp2)))
				{
					_bloodAddr = ret + nDeltaBlood32;
					_crazyAddr = ret + nDeltaCrazy32;
					LOGE("+++update DadNMe(alternative) _bloodAddr _crazyAddr\n");
				}
			}
			else//连续两次出现时
			{
				//1065353216 1146129076
				_bloodAddr = ret + nDeltaBlood32;
				_crazyAddr = ret + nDeltaCrazy32;
				LOGE("+++update DadNMe _bloodAddr _crazyAddr: %x %x %d %d\n", _bloodAddr, _crazyAddr, 
				*(static_cast<unsigned int*>(_bloodAddr)), *(static_cast<unsigned int*>(_crazyAddr)));
				/*unsigned int* addrTmp1 = static_cast<unsigned int*>(_bloodAddr);
				unsigned int* addrTmp2 = static_cast<unsigned int*>(_crazyAddr);
				LOGE("value: %d %d\n", (int)(*addrTmp1), (int)(*addrTmp2));*/
			}
		}
		
代码中的判断：
if ((1065353216 == (*addrTmp1)) && (1146129076 == (*addrTmp2)))
不同手机没测试过初值是不是相同
```