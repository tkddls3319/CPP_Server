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

void Session::Send(BYTE* buffer, int32 len)
{
	//������ ����
	//1. ���� ����
	//2. sendEvent ���� ����? ������? WSASend ��ø?

	SendEvent* sendEvent = xnew<SendEvent>();
	sendEvent->owner = shared_from_this(); /*ADD_BEF*/
	sendEvent->buffer.resize(len);
	::memcpy(sendEvent->buffer.data(), buffer, len);

	//�۽� ���
	WRITE_LOCK;//send�� ��Ƽ������ ȯ�濡�� �������� �������� �ʴ´�.(����x)
	RegisterSend(sendEvent);
}

void Session::Disconnect(const WCHAR* cause)
{
	if (_connected.exchange(false) == false)
		return;

	wcout << "Disconnect : " << cause << endl;

	OnDisconnected();//������ �ڵ� �����ε�
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
		ProcessSend(static_cast<SendEvent*>(iocpEvent), numOfBytes);
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
			_recvEvent.owner = nullptr;//release_ref �����ָ� �޸� �ҵ�
		}
	}
}

void Session::RegisterSend(SendEvent* sendEvent)
{
	if (IsConnected() == false)
		return;

	WSABUF wsaBuf;
	wsaBuf.buf = (char*)sendEvent->buffer.data();
	wsaBuf.len = (ULONG)sendEvent->buffer.size();

	DWORD numOfBytes = 0;

	if (SOCKET_ERROR == ::WSASend(_socket, &wsaBuf, 1, OUT &numOfBytes, 0, sendEvent, nullptr))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)//Ŀ�ο� ������۰� ��á�ٸ� pending�� �Ͼ.
		{
			HandleError(errorCode);
			sendEvent->owner = nullptr;//release_ref
			xdelete(sendEvent);
		}
	}

}

void Session::ProcessConnect()
{
	_connected.store(true);

	//���ǵ��
	GetService()->AddSession(GetSessionRef());

	//������ �ڵ忡�� �����ε�
	OnConnected();

	//���� ���
	RegisterRecv();

}

void Session::ProcessRecv(int32 numOfBytes)
{
	_recvEvent.owner = nullptr;//����� ����Ʈ ���� release_ref �����ָ� �޸� �ҵ�

	if (numOfBytes == 0)
	{
		Disconnect(L"recv 0");
		return;
	}

	OnRecv(_recvBuffer, numOfBytes);

	//�� ���ŵ��
	RegisterRecv();
}

void Session::ProcessSend(SendEvent* sendEvent, int32 numOfBytes)
{
	sendEvent->owner = nullptr;//����� ����Ʈ ���� release_ref �����ָ� �޸� �ҵ�
	xdelete(sendEvent);

	if (numOfBytes == 0)
	{
		Disconnect(L"Send 0");
		return;
	}

	//���������� �����ε� ����Ϸ�
	OnSend(numOfBytes);
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
