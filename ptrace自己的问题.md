#

```
 ptrace(PTRACE_TRACEME,0,NULL,NULL);
 可以用做反调试
```

```
int keep_blood()
{
	if (0 == hookDadNMe::_curPid)
		return -1;
	if (nullptr == hookDadNMe::_bloodAddr)//_bloodAddr还可以做范围检测
		return -1;
	/*pid_t target_pid = hookDadNMe::_curPid;
	int status;
	LOGE("#########	attach %d %d %d\n", hookDadNMe::_curPid, find_pid_of("com.lakegame.dadnme"), target_pid);
	if (ptrace(PTRACE_ATTACH, target_pid, NULL, NULL) == -1L)//so 打入app中，so和app在一个进程中；这样attach失败，不知道是不是写法有问题???
	//确认给了进程是给了root权限的，在MainActivity的OnCreate函数中调用了 Shell.isSuAvailable();
	//如果单独起一个进程，给root权限，或者在Virual Xposed、夜神模拟器中是可以attach的
	{
		LOGE("failed to attach\n");
		LOGE("errno %s %d\n", strerror(errno), errno);//errno Operation not permitted 1
        return -1;
    }
	if (waitpid(target_pid, &status, 0) == -1 || !WIFSTOPPED(status))
	{
		LOGE("there was an error waiting for the target to stop.\n");
        return -1;
    }
	
	char buf[] = { 0x40, 0x51, 0x80, 0x00 };
	void *data = buf;
	

	long ptraced_long = ptrace(PTRACE_PEEKDATA, target_pid, hookDadNMe::_bloodAddr, NULL);
	if (1)
	{
		LOGE("ptraced_long	%d\n", ptraced_long);
	}

	if (ptrace(PTRACE_POKEDATA, target_pid, hookDadNMe::_bloodAddr, *(long *)(data)) == -1L)	
	{
		LOGE("memory allocation failed 1.\n");
		return -1;
    }

	LOGE("#########	detach\n");
	ptrace(PTRACE_DETACH, target_pid, 1, 0);*/
	//so和app在同一个进程，其实可以直接访问内存
	unsigned int* addr = static_cast<unsigned int*>(hookDadNMe::_bloodAddr);
	*addr = 1079083008;//0x405180000h;1079083008D;3.2734375F
	return 0;
}

```