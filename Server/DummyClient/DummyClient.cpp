#include "pch.h"
#include <iostream>
#include "CorePch.h"

#include <WinSock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

void HandleError(const char* cause)
{
	int32 errCode = ::WSAGetLastError();
	cout << cause << " ErrorCode" << errCode << endl;
}
int main()
{

	this_thread::sleep_for(1s);

	//윈소켓 초기화(w2_32 라이브러리 초기화)
	//관련정보가 wsaData에 채워짐
	WSADATA wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 0;

	SOCKET clientSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == INVALID_SOCKET)
		return 0;

	//논블로킹을 하기위해
	u_long on = 1;
	if (::ioctlsocket(clientSocket, FIONBIO, &on) == INVALID_SOCKET)
		return 0;

	//나의 주소 : IP주소 + PORT
	SOCKADDR_IN serverAddr;//IPv4
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	::inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
	serverAddr.sin_port = ::htons(7777);


	//Connect
	while (true)
	{
		if (::connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) 
		{
			//원래 블록했어야함.
			if (::WSAGetLastError() == WSAEWOULDBLOCK)
				continue;

			//이미 연결된 상태라면 break;
			if (::WSAGetLastError() == WSAEISCONN)
				break;

			//ERROR
			break;
		}
	}
	cout << "Server Connected!" << endl;

	char sendBuffer[100] = "Hello World";
	WSAEVENT wsaEvent = ::WSACreateEvent();
	WSAOVERLAPPED overlapped = {};
	overlapped.hEvent = wsaEvent;

	while (true)
	{
		WSABUF wsaBuf;
		wsaBuf.buf = sendBuffer;
		wsaBuf.len = 100;

		DWORD sendLen = 0;
		DWORD flags = 0;

		if (::WSASend(clientSocket, &wsaBuf, 1, &sendLen, flags, &overlapped, nullptr) == SOCKET_ERROR)
		{
			if (::WSAGetLastError() == WSA_IO_PENDING)
			{
				//pending  지연
				::WSAWaitForMultipleEvents(1, &wsaEvent, TRUE, WSA_INFINITE, FALSE);//결과 받을 때 까지 대기
				::WSAGetOverlappedResult(clientSocket, &overlapped, &sendLen, FALSE, &flags);
			}
			else
			{
				//TODO : 문제 상황
			}
		}
	
		cout << "send Data Len" << sizeof(sendBuffer) << endl;
		this_thread::sleep_for(1s);
	}

	//소켓 리소스 반환
	::closesocket(clientSocket);

	//윈도우 소켓 종료 WSADATA호출 수만큼 정리해줘야함(권고사항)
	::WSACleanup();
}
