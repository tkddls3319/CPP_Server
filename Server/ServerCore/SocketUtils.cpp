#include "pch.h"
#include "SocketUtils.h"

/*------------------
 SocketUtils
------------------*/

//비동기 소켓 운영을 위한 함수의 포인터들 (델리게이트)
LPFN_CONNECTEX SocketUtils::ConnectEx = nullptr;
LPFN_DISCONNECTEX SocketUtils::DisConnectEx = nullptr;
LPFN_ACCEPTEX SocketUtils::AcceptEx = nullptr;

void SocketUtils::Init()
{
	WSADATA wsaData;
	ASSERT_CRASH(::WSAStartup(MAKEWORD(2, 2), OUT & wsaData) == 0);

	//런타임에 주소 얻어오기
	SOCKET dummySocket = CreateSocket();
	ASSERT_CRASH(BindWindowsFunction(dummySocket, WSAID_CONNECTEX, reinterpret_cast<LPVOID*>(&ConnectEx)));
	ASSERT_CRASH(BindWindowsFunction(dummySocket, WSAID_DISCONNECTEX, reinterpret_cast<LPVOID*>(&DisConnectEx)));
	ASSERT_CRASH(BindWindowsFunction(dummySocket, WSAID_ACCEPTEX, reinterpret_cast<LPVOID*>(&AcceptEx)));
}

void SocketUtils::Clear() {

	::WSACleanup();
}

bool SocketUtils::BindWindowsFunction(SOCKET socket, GUID guid, LPVOID* fn)
{
	DWORD bytes = 0;

	//ConnectEx, DisConnectEx, AcceptEx 를 런타임에 불러오도록 설정
	return SOCKET_ERROR != ::WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(guid), fn, sizeof(*fn), OUT & bytes, NULL, NULL);
}

SOCKET SocketUtils::CreateSocket()
{
	return	::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
}

bool SocketUtils::SetLinger(SOCKET socket, uint16 onoff, uint16 linger)
{
	LINGER option;
	option.l_onoff = onoff;
	option.l_linger = linger;
	return SetSockOpt(socket, SOL_SOCKET, SO_LINGER, option);
}

// 주소 재사용을 허용
bool SocketUtils::SetReuseAddress(SOCKET socket, bool flag)
{
	return SetSockOpt(socket, SOL_SOCKET, SO_REUSEADDR, flag);
}
//수신 버퍼 크기를 조정
bool SocketUtils::SetRecvBufferSize(SOCKET socket, int32 size)
{
	return SetSockOpt(socket, SOL_SOCKET, SO_RCVBUF, size);
}
//송신 버퍼 크기를 조정
bool SocketUtils::SetSendBufferSize(SOCKET socket, int32 size)
{
	return SetSockOpt(socket, SOL_SOCKET, SO_SNDBUF, size);
}
//Nagle 알고리즘을 비활성화합니다. 
bool SocketUtils::SetTcpNoDelay(SOCKET socket, bool flag)
{
	return SetSockOpt(socket, SOL_SOCKET, TCP_NODELAY, flag);
}

//이미 수락된 소켓에 대한 상위 수락 소켓의 컨텍스트를 업데이트합니다.
//즉 Listensocket의 특성을 새로운 socket에 그대로 적용합니다.
bool SocketUtils::SetUpdateAcceptSocket(SOCKET socket, SOCKET listenSocket)
{
	return SetSockOpt(socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, listenSocket);
}

bool SocketUtils::Bind(SOCKET socket, NetAddress netAddr)
{
	return SOCKET_ERROR != ::bind(socket, reinterpret_cast<const SOCKADDR*>(&netAddr.GetSockAddr()), sizeof(SOCKADDR_IN));
}

bool SocketUtils::BindAnyAddress(SOCKET socket, uint16 port)
{
	//나의 주소 : IP주소 + PORT
	SOCKADDR_IN myAddress;//IPv4
	myAddress.sin_family = AF_INET;
	myAddress.sin_addr.s_addr = ::htonl(INADDR_ANY);//아이피 알아서 설정
	myAddress.sin_port = ::htons(port);//PORT

	return SOCKET_ERROR != ::bind(socket, reinterpret_cast<const SOCKADDR*> (&myAddress), sizeof(myAddress));
}

bool SocketUtils::Listen(SOCKET socket, int32 backlog)
{
	return SOCKET_ERROR != listen(socket, backlog);
}

void SocketUtils::Close(SOCKET socket)
{
	if(socket != INVALID_SOCKET)
	::closesocket(socket);

	socket = INVALID_SOCKET;
}
