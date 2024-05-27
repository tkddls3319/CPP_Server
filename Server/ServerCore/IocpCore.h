#pragma once

/*------------------
 IocpCore
------------------*/
class IocpCore
{
public:

	IocpCore();
	~IocpCore();

	HANDLE GetHandle() { return _iocpHandle; }

	bool Register();
	bool Dispatch(uint32 timeoutMs = INFINITE);;

private:
	HANDLE _iocpHandle;
};

