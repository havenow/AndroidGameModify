#
```
int main(int argc, char** argv)
{
	static char ori_buf1[] = { 0x01, 0xAD, 0x52, 0xE1 };//高字节-->低字节 0xE1 0x52 0xAD 0x01
	long *dataTest;
	dataTest = reinterpret_cast<long*>(ori_buf1);//dataTest的值是 -514675455
	
	return 0;
}
```

#
```
用GG修改器获取的地址0x9D51CE40处的四字节数据是						E5802020
																			将E5802020看成字符串，尾端一个字节的数据是20
arm用的小端模式（低尾端，尾端的数据存放在低地址），低地址-->高地址	20 20 80 E5

如果将地址0x9D51CE40处的四字节数据 E5802020 看成一个四字节的整型
char buf[] = { 0x20, 0x20, 0x80, 0xE5 };
long lb = reinterpret_cast<long*>(buf);//lb的值是 -444588000

```