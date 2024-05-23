#include "pch.h"
#include <iostream>
#include "CorePch.h"

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
	SOCKET clientSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == INVALID_SOCKET)
	{
		int32 errCode = ::WSAGetLastError();
		cout << "socket error code" << errCode << endl;
		return 0;
	}

	//연결할 목적지 : IP주소 + PORT
	SOCKADDR_IN serverAddr;//IPv4
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	//serverAddr.sin_addr.s_addr = ::inet_addr("127.0.0.1");
	::inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
	serverAddr.sin_port = ::htons(7777);//PORT
	//htons(host to network short) Little-Endian, Big-Endian

	//서버에 연결
	if (::connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		int32 errCode = ::WSAGetLastError();
		cout << "Connect error code" << errCode << endl;
		return 0;
	}

	//--------------------------
	///연결성공

	cout << "Connected To Server!" << endl;

	while (true)
	{
		char sendBuffer[100] = "Hellow World!";
		int32 resultCode = ::send(clientSocket, sendBuffer, sizeof(sendBuffer), 0);
		if (resultCode == SOCKET_ERROR)
		{
			int32 errCode = ::WSAGetLastError();
			cout << "Send error code" << errCode << endl;
			return 0;
		}

		cout << "Send Data Len = " << sizeof(sendBuffer) << endl;

		char recvBuffer[1000];
		int32 recvLen = ::recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);

		if (recvLen <= 0)
		{
			int32 errCode = ::WSAGetLastError();
			cout << "Recv error code" << errCode << endl;
			return 0;
		}
		cout << "Recv Data = " << recvBuffer << endl;

		this_thread::sleep_for(1s);
	}
	//--------------------------

	//소켓 리소스 반환
	::closesocket(clientSocket);

	//윈도우 소켓 종료 WSADATA호출 수만큼 정리해줘야함(권고사항)
	::WSACleanup();
}
