#include "pch.h"
#include "Session.h"
#include "SocketUtils.h"
#include "Service.h"
/*--------------
	Session
---------------*/

Session::Session()
{
	_socket = SocketUtils::CreateSocket();
}

Session::~Session()
{
	SocketUtils::Close(_socket);
}

void Session::Disconnect(const WCHAR* cause)
{
	if (_connected.exchange(false) == false)
		return;

	wcout << "Disconnect : " << cause << endl;

	OnDisConnected();//컨텐츠 코드 오버로딩
	SocketUtils::Close(_socket);
	GetService()->ReleaseSession(GetSessionRef());
}

HANDLE Session::GetHandle()
{
	return reinterpret_cast<HANDLE>(_socket);
}

void Session::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
{
	// TODO
	switch (iocpEvent->eventType)
	{
	case EventType::Connect:
		ProcessConnect();
		break;
	case EventType::Recv:
		ProcessRecv(numOfBytes);
		break;
	case EventType::Send:
		ProcessSend(numOfBytes);
		break;
	default:
		break;
	}
}

void Session::RegisterConnect()
{
}

void Session::RegisterRecv()
{
	if (IsConnected() == false)
		return;

	_recvEvent.Init();
	_recvEvent.owner = shared_from_this();//add_ref

	WSABUF wsaBuf;
	wsaBuf.buf = reinterpret_cast<char*>(_recvBuffer);
	wsaBuf.len = len32(_recvBuffer);

	DWORD numOfBytes = 0;
	DWORD flags = 0;

	if (SOCKET_ERROR == ::WSARecv(_socket, &wsaBuf, 1, OUT &numOfBytes, OUT &flags, &_recvEvent, nullptr))
	{
		int32 errorCode = ::WSAGetLastError();

		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			_recvEvent.owner = nullptr;//release_ref 안해주면 메모리 팬딩
		}
	}
}

void Session::RegisterSend()
{
}

void Session::ProcessConnect()
{
	_connected.store(true);

	//세션등록
	GetService()->AddSession(GetSessionRef());

	//컨텐츠 코드에서 오버로딩
	OnConnected();

	//수신 등록
	RegisterRecv();
}

void Session::ProcessRecv(int32 numOfBytes)
{
	_recvEvent.owner = nullptr;//쉐어드 포인트 여서 release_ref 안해주면 메모리 팬딩

	if (numOfBytes == 0)
	{
		Disconnect(L"recv 0");
		return;
	}

	cout << "recv data len =" << numOfBytes << endl;

	//재 수신등록
	RegisterRecv();
}

void Session::ProcessSend(int32 numOfBytes)
{
}

void Session::HandleError(int32 errorCode)
{
	switch (errorCode)
	{
	case WSAECONNRESET:
		break;
	case WSAECONNABORTED:
		Disconnect(L"HandleError");
		break;
	default:
		cout << "handle error" << errorCode << endl;
		break;
	}
}
