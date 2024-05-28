#pragma once
#include "IocpCore.h"
#include "IocpEvent.h"
#include "NetAddress.h"

/*------------------
	 Session
------------------*/

class Session : public IocpObject
{
public:
	Session();
	virtual ~Session();


public:
	//세션정보
	void SetNetAddress(NetAddress address) { _netAddress = address; }
	NetAddress GetAddress() { return _netAddress; }
	SOCKET GetSocket() { return _socket; }

public:
	//인터페이스 구현
	virtual HANDLE GetHandle() override;
	virtual void Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;

public:
	char _recvBuffer[1000];


private:

	SOCKET _socket = INVALID_SOCKET;
	NetAddress _netAddress = {};//접속정보
	Atomic<bool> _connected = false;


};

