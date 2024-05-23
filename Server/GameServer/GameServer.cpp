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

int main()
{
	//윈소켓 초기화(w2_32 라이브러리 초기화)
	//관련정보가 wsaData에 채워짐
	WSADATA wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 0;

	//af : Address Family ( AF_INET = IPv4, AF_INET6 = IPv6
	//type : TCP(SOCK_STREAM), UDP(SOCK_DGRAM)
	//protocol : 0
	//return : 성공여부
	SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET)
	{
		int32 errCode = ::WSAGetLastError();
		cout << "socket error code" << errCode << endl;
		return 0;
	}

	//나의 주소 : IP주소 + PORT
	SOCKADDR_IN serverAddr;//IPv4
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);//아이피 알아서 설정
	serverAddr.sin_port = ::htons(7777);//PORT
	//htons(host to network short) Little-Endian, Big-Endian

	//socket과 주소 연동
	if (::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)))
	{
		int32 errCode = ::WSAGetLastError();
		cout << "Bind error code" << errCode << endl;
		return 0;
	}

	//서버오픈
	if (::listen(listenSocket, 10) == SOCKET_ERROR)//10은 대기열 이걸넘으면 못들어옴
	{
		int32 errCode = ::WSAGetLastError();
		cout << "Listen error code" << errCode << endl;
		return 0;
	}

	//--------------------------
	///오픈성공
	while (true)
	{
		SOCKADDR_IN clientAddr;//IPv4
		::memset(&clientAddr, 0, sizeof(clientAddr));
		int32 addrLen = sizeof(clientAddr);
		SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);

		if (clientSocket == INVALID_SOCKET)
		{
			int32 errCode = ::WSAGetLastError();
			cout << "Accept error code" << errCode << endl;
			return 0;
		}

		//클라이언트 입장
		char ipAddress[16];
		::inet_ntop(AF_INET, &clientAddr.sin_addr, ipAddress, sizeof(ipAddress));

		cout << "Client Connected! IP = " << ipAddress << endl;
	
		while (true)
		{
			char recvBuffer[1000];
			int32 recvLen = ::recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);

			if (recvLen <= 0)
			{
				int32 errCode = ::WSAGetLastError();
				cout << "Recv error code" << errCode << endl;
				return 0;
			}

			cout << "Recv Data = " << recvBuffer << endl;

			int32 resultCode = ::send(clientSocket, recvBuffer, recvLen, 0);
			if (resultCode == SOCKET_ERROR)
			{
				int32 errCode = ::WSAGetLastError();
				cout << "Send error code" << errCode << endl;
				return 0;
			}
		}
	}
	//--------------------------

	//윈도우 소켓 종료 WSADATA호출 수만큼 정리해줘야함(권고사항)
	::WSACleanup();
}