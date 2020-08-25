# 加载jni.h
```
IDA File/Load file/Parse C header file... Ctrl+F9
```

# 如何判断so的指令集是thumb还是arm
```
IDA中看相邻两个汇编指令的地址，如果地址相差2，就是thumb指令集
Options/General/Disassembly/Number of opcode bytes(non-grahp)	将0改为4，地址旁边会显示指令码
```

