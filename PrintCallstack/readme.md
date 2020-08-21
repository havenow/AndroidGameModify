#说明

```
参考
https://blog.csdn.net/taohongtaohuyiwei/article/details/105147933

Application.mk中APP_STL设置成gnustl_static，不然Unwind相关的编译不通过
#APP_STL := stlport_static
APP_STL := gnustl_static
```

```
libCLEO.so里面的调用堆栈没有打印，原因？？？

    backtrace:
    #00 pc 00004c15  /data/app/com.rockstargames.gtasa-DU1TjY3-wT5Qq_L9Tw88pQ==/lib/arm/libinlineHook.so (_Z13callstackDumpRSs+00)
    #01 pc 00004cd1  /data/app/com.rockstargames.gtasa-DU1TjY3-wT5Qq_L9Tw88pQ==/lib/arm/libinlineHook.so (_Z15callstackLogcatiPKc+00)
    #02 pc 00004d1d  /data/app/com.rockstargames.gtasa-DU1TjY3-wT5Qq_L9Tw88pQ==/lib/arm/libinlineHook.so (_Z19replaceWeaponCheat1Pv+00)
    #03 pc 00007ecf  /data/app/com.rockstargames.gtasa-DU1TjY3-wT5Qq_L9Tw88pQ==/lib/arm/libCLEO.so
```