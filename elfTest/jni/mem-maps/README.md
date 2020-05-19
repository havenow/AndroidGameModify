# 说明
/proc/uid/maps     
/proc/uid/exe      

通过readlink读/proc/uid/exe，可以获取exename（"/system/bin/app_process32"）   
通过解析 /proc/uid/maps，可以获取需要搜索的内存块，内存块的类型（可以查看regions.txt文件）     


REGION_TYPE_CODE    

```
b72a1000-b7354000 r-xp 00000000 08:06 796        /system/lib/libEGL.so（注意有x权限）
b7354000-b7356000 r--p 000b2000 08:06 796        /system/lib/libEGL.so
b7356000-b735d000 rw-p 000b4000 08:06 796        /system/lib/libEGL.so
b735d000-b7360000 rw-p 00000000 00:00 0 							    (注意是rw)

b735d000-b7360000 是REGION_TYPE_CODE类型的内存块
```

REGION_TYPE_EXE    

```
b775b000-b775f000 r-xp 00000000 08:06 84         /system/bin/app_process32（注意有x权限）
b775f000-b7760000 r--p 00003000 08:06 84         /system/bin/app_process32
b7760000-b7761000 rw-p 00000000 00:00 0 									(注意是rw)

b7760000-b7761000 是REGION_TYPE_EXE类型的内存块
```
