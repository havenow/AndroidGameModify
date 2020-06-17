# 

```
https://blog.csdn.net/h523669057/article/details/42876119
1、此代码网上下载的，只支持32位进程，如果是64位进程注入会失败，因为代码中使用了regs->ARM_pc

2、libc_path linker_path 
在/proc/uid/maps文件中可以查看到
const char *libc_path = "/apex/com.android.runtime/lib/bionic/libc.so";    
const char *linker_path = "/apex/com.android.runtime/lib/bionic/libdl.so";  

原来的代码太老：
const char *libc_path = "/system/lib/libc.so";    
const char *linker_path = "/system/bin/linker"; 

3、根据包名获取进程号
pid_t target_pid = find_pid_of("com.and.games505.TerrariaPaid");    

4、注入so到进程中
先push libhello.so到/data/local/tmp目录下，然后进入adb shell ，su, cp libhello.so到/data/app/com.and.games505.TerrariaPaid-zEFfgdCC4F8y6CFw-m7lNw==/lib/arm/ 下
注意这里的libhello.so要拷贝到/data/app/com.and.games505.TerrariaPaid-zEFfgdCC4F8y6CFw-m7lNw==/lib/arm/ 下
目录的获取是查看/proc/uid/maps文件中的其他so存放的目录，这个目录是app安装之后就不会改变的
cat /proc/19426/maps | grep libunity	看到libunity.so存放的目录就是需要拷贝的目录

inject_remote_process(target_pid, "libhello.so", "hook_entry",  "I'm parameter!", strlen("I'm parameter!"));  

原来的代码是：
inject_remote_process(target_pid, "/data/libhello.so", "hook_entry",  "I'm parameter!", strlen("I'm parameter!"));    

```