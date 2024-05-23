#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <atomic>
#include <mutex>
#include <windows.h>
#include <future>
#include "ThreadManager.h"

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
	//윈소켓 초기화(w2_32 라이브러리 초기화)
	//관련정보가 wsaData에 채워짐
	WSADATA wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 0;

	SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET)
		return 0;

	//논블로킹을 하기위해
	u_long on = 1;
	if (::ioctlsocket(listenSocket, FIONBIO, &on) == INVALID_SOCKET)
		return 0;

	//나의 주소 : IP주소 + PORT
	SOCKADDR_IN serverAddr;//IPv4
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);//아이피 알아서 설정
	serverAddr.sin_port = ::htons(7777);//PORT

	//socket과 주소 연동
	if (::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
		return 0;

	//서버오픈
	if (::listen(listenSocket, 10) == SOCKET_ERROR)//10은 대기열 이걸넘으면 못들어옴
		return 0;

	cout << "Accept" << endl;

	SOCKADDR_IN clientAddr;//IPv4
	::memset(&clientAddr, 0, sizeof(clientAddr));
	int32 addrLen = sizeof(clientAddr);


	while (true)
	{
		SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
		if (clientSocket == INVALID_SOCKET)
		{
			//원래 블록했어야함.
			if (::WSAGetLastError() == WSAEWOULDBLOCK)
				continue;

			//Error
			break;
		}
		cout << "Client Connected!" << endl;

		while (true)
		{
			char recvBuffer[1000];
			int32 recvLen = ::recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
			if (recvLen == SOCKET_ERROR)
			{
				//원래 블록했어야함.
				if (::WSAGetLastError() == WSAEWOULDBLOCK)
					continue;

				//Error
				break;
			}
			else if (recvLen == 0)
			{
				//연결끊김
				break;
			}

			cout << "Recv Data Len = " << recvLen << endl;

			while (true)
			{
				if (::send(clientSocket, recvBuffer, recvLen, 0) == SOCKET_ERROR)
				{
					//원래 블록했어야함.
					if (::WSAGetLastError() == WSAEWOULDBLOCK)
						continue;

					//Error
					break;
				}

				cout << "send Data Len" << recvLen<< endl;
				break;
			}
		}
	}

	::WSACleanup();
}