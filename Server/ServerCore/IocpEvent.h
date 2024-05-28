#pragma once


class Session;

enum class EventType : uint8
{
	Connect,
	Accept,
	//PreRecv,
	Recv,
	Send
};
/*------------------
 IocpEvent
------------------*/
class IocpEvent: public OVERLAPPED
{
public:
	IocpEvent(EventType type);

	void Init();
	EventType GetType() { return _type; }

protected:
	EventType _type;
};

/*------------------
 ConnectEvent
------------------*/
class ConnectEvent :public IocpEvent
{
public:
	ConnectEvent() :IocpEvent(EventType ::Connect) { }
};
/*------------------
 AcceptEvent
------------------*/
class AcceptEvent :public IocpEvent
{
public:
	AcceptEvent() :IocpEvent(EventType::Accept) { }

	void SetSession(Session* session) { _session = session; }

	Session* GetSession() { return _session; }

private:
	Session* _session = nullptr;
};
/*------------------
 RecvEvent
------------------*/
class RecvEvent :public IocpEvent
{
public:
	RecvEvent() :IocpEvent(EventType::Recv) { }

private:
	//TODO 인자가 추가적으로 있을수도있다.
};
/*------------------
SendEvent
------------------*/
class SendEvent :public IocpEvent
{
public:
	SendEvent() :IocpEvent(EventType::Send) { }

private:
	//TODO 인자가 추가적으로 있을수도있다.
};