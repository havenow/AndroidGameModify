#ifndef _HOOKSTRATEGY_H
#define _HOOKSTRATEGY_H

class hookStrategy
{
public:
	hookStrategy(){}
	virtual ~hookStrategy(){}
	
	virtual void calFunAddr(void* dll) = 0;
	virtual void DoHook() = 0;
};

class hookGTASA : public hookStrategy
{
public:
	hookGTASA();
	virtual ~hookGTASA();
	
	void calFunAddr(void* dll);
	virtual void DoHook();

protected:
	static int replaceGameProcess();
	int hookGameProcess();
	int unHook();
	long funGameProcessdAddr;
	int (*f__ZN5CGame7ProcessEv)();
	
};

class hookGTAVC : public hookStrategy
{
public:
	hookGTAVC();
	virtual ~hookGTAVC();
	
	void calFunAddr(void* dll);
	virtual void DoHook();

protected:
	static int replaceGameProcess();
	int hookGameProcess();
	int unHook();
	long funGameProcessdAddr;
	int (*f__ZN5CGame7ProcessEv)();
	
};

class hookGTALCS : public hookStrategy
{
public:
	hookGTALCS();
	virtual ~hookGTALCS();
	
	void calFunAddr(void* dll);
	virtual void DoHook();
	
protected:
	static int replaceGameProcess(float arg);
	int hookGameProcess();
	int unHook();
	long funGameProcessdAddr;
	int (*f__ZN5CGame7ProcessEf)(float); 
};

class hookGTA3 : public hookStrategy
{
public:
	hookGTA3();
	virtual ~hookGTA3();
	
	void calFunAddr(void* dll);
	virtual void DoHook();

protected:
	static int replaceGameProcess();
	int hookGameProcess();
	int unHook();
	long funGameProcessdAddr;
	int (*f__ZN5CGame7ProcessEv)();	
};

class hookGame
{
public:
	hookGame(){}
	hookGame(hookStrategy* pStrategy)	{ this->_pHookStrategy = pStrategy; }
	~hookGame(){}
	
	void setStrategy(hookStrategy* pStrategy) {this->_pHookStrategy = pStrategy;}
	void setDll(void* dll) { this->_pDll = dll;}
	void DoHook()
	{
		if (_pHookStrategy)
		{
			_pHookStrategy->calFunAddr(_pDll);
			_pHookStrategy->DoHook();
		}
	}

private:
	hookStrategy* _pHookStrategy = nullptr;
	void* _pDll = nullptr;
};

#endif