#
```
so 在virtual app中直接使用，游戏是子进程，父进程ptrace子进程
问题：
安卓上层调用Java_io_virtualapp_home_NativePtrace_gameStarted轮询游戏是否已经启动时
find_pid_of函数在读文件/proc/%d/cmdline，需要看做是临界资源
```