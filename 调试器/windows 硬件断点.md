# 使用GetThreadContext/SetThreadContext获得寄存器信息
```
每个线程都有一个上下文环境，它包含了有关线程的大部分信息，例如线程栈的地址，线程当前正在执行的指令地址等。
上下文环境保存在寄存器中，系统进行线程调度的时候会发生上下文切换，实际上就是将一个线程的上下文环境保存到内存中，
然后将另一个线程的上下文环境装入寄存器。

typedef struct _CONTEXT {

    DWORD ContextFlags;
    DWORD   Dr0;
    DWORD   Dr1;
    DWORD   Dr2;
    DWORD   Dr3;
    DWORD   Dr6;
    DWORD   Dr7;
    FLOATING_SAVE_AREA FloatSave;
    DWORD   SegGs;
    DWORD   SegFs;
    DWORD   SegEs;
    DWORD   SegDs;
    DWORD   Edi;
    DWORD   Esi;
    DWORD   Ebx;    
    DWORD   Edx;
    DWORD   Ecx;
    DWORD   Eax;
    DWORD   Ebp;    //栈底
    DWORD   Eip;     //下一条执行指令
    DWORD   SegCs;              // MUST BE SANITIZED
    DWORD   EFlags;             // MUST BE SANITIZED
    DWORD   Esp;   //栈顶
    DWORD   SegSs;  //SS
    BYTE    ExtendedRegisters[MAXIMUM_SUPPORTED_EXTENSION];

} CONTEXT;

获取某个线程的上下文环境需要使用GetThreadContext函数
调用GetThreadContext函数之后，CONTEXT结构相应的字段就会被赋值，此时就可以输出各个寄存器的值了。

使用SetThreadContext写入寄存器值 
```

# 硬件断点的实现
```
DRx调试寄存器总共有8个，从DRx0到DRx7。每个寄存器的特性如下：
　　　　DR0~DR3：调试地址寄存器，保存需要监视的地址，如设置硬件断点；
　　　　DR4~DR5：保留，未公开具体作用；
　　　　DR6：调试寄存器组状态寄存器，控制哪个寄存器被命中
　　　　DR7：控制着哪个DRx设置的断点，局部或者全局，读写/执行/写断点类型，断点长度1/2/4/8,的信息

void SetBits(DWORD_PTR& dw, int lowBit, int bits, int newValue)
{
    DWORD_PTR mask = (1 << bits) - 1; 
    dw = (dw & ~(mask << lowBit)) | (newValue << lowBit);
}

/*
第一个参数HANDLE hThread：
HANDLE g_hThread = NULL;
STARTUPINFO si = { 0 };
si.cb = sizeof(si);
PROCESS_INFORMATION pi = { 0 };
	if (CreateProcess(
		szFilePath,
		NULL,
		NULL,
		NULL,
		FALSE,
		DEBUG_ONLY_THIS_PROCESS | CREATE_NEW_CONSOLE,//注意这里的参数
		NULL,
		NULL,
		&si,
		&pi) == FALSE) {
			MessageBox(NULL,L"CreateProcess failed",L"MZ",1);
			return;
	}

	g_hProcess = pi.hProcess;
	g_hThread = pi.hThread;

第四个参数void* s：
ULONG Address = 0;//手动输入

调用SetHardwareBreakpoint：
SetHardwareBreakpoint(g_hThread, HWBRK_TYPE_CODE, HWBRK_SIZE_1, (void*)Address);
*/
HANDLE SetHardwareBreakpoint(HANDLE hThread,HWBRK_TYPE Type,HWBRK_SIZE Size,void* s)
{ 
	...
	SuspendThread(hThread);
	
	//获取线程的上下文环境
	CONTEXT ct = {0};
	ct.ContextFlags = CONTEXT_DEBUG_REGISTERS|CONTEXT_FULL;
	GetThreadContext(hThread,&ct);
	
	//根据HWBRK_TYPE Type,HWBRK_SIZE Size确定st le
	int st = 0;
	if (Type == HWBRK_TYPE_CODE)
        st = 0;
	int le = 0;
	if (Size == HWBRK_SIZE_1)
        le = 0;
	
	//设置调试寄存器
	ct.Dr0 = (DWORD_PTR)s;  //地址
	ct.Dr6 = 0;
	SetBits(ct.Dr7, 16 + iReg*4, 2, st);
    SetBits(ct.Dr7, 18 + iReg*4, 2, le);
    SetBits(ct.Dr7, iReg*2,1,1);
	
	//设置线程的上下文环境
	ct.ContextFlags = CONTEXT_DEBUG_REGISTERS;
	SetThreadContext(hThread,&ct);
	
	ResumeThread(hThread);
	...
}
```

#处理硬件断点
```
ContinueDebugSession(step in/out/over等各种调试场景会调用)
DispatchDebugEvent
OnException
OnSingleStep


void ContinueDebugSession() {
	while (WaitForDebugEvent(&debugEvent, INFINITE) == TRUE) {//循环
		if (DispatchDebugEvent(&debugEvent) == TRUE) {

//根据调试事件的类型调用不同的处理函数。
BOOL DispatchDebugEvent(const DEBUG_EVENT* pDebugEvent) {
	switch (pDebugEvent->dwDebugEventCode) {
		case EXCEPTION_DEBUG_EVENT:
		{
			return OnException	(&pDebugEvent->u.Exception);
		}
	

//发生异常的时候应该通知用户，交由用户来处理，所以应返回FALSE。
BOOL OnException(const EXCEPTION_DEBUG_INFO* pInfo) {
	case EXCEPTION_BREAKPOINT:
		{
			/*OnShowSourceLines();*/
			
			return OnSoftBreakPoint(pInfo);   //return false 则中断停下来
		}

	case EXCEPTION_SINGLE_STEP:  
		{
			/*OnShowSourceLines();*/
			
			return OnSingleStep(pInfo);
		}
}


BOOL OnSingleStep(const EXCEPTION_DEBUG_INFO* pInfo)函数中

	CONTEXT Context = {0};
	GetDebuggeeContext(&Context);
	for(DWORD i = 0; i < m_vecHard.size( ); ++i)
	{
		if(m_vecHard[i].lpPointAddr ==(DWORD)pInfo->ExceptionRecord.ExceptionAddress)
		{
			CString strStatusMsg;
			strStatusMsg.Format(L"A Hard break point occured at %p",(DWORD)pInfo->ExceptionRecord.ExceptionAddress);
	
	...
	
	
```



