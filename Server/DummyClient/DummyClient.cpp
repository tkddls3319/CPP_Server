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
	//윈소켓 초기화(w2_32 라이브러리 초기화)
	//관련정보가 wsaData에 채워짐
	WSADATA wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 0;

	//af : Address Family ( AF_INET = IPv4, AF_INET6 = IPv6
	//type : TCP(SOCK_STREAM), UDP(SOCK_DGRAM)
	//protocol : 0
	//return : 성공여부
	SOCKET clientSocket = ::socket(AF_INET, SOCK_DGRAM, 0);
	if (clientSocket == INVALID_SOCKET)
	{
		HandleError("Socket");
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

	//Connected UDP tcp처럼 사용(실제로연결된건아님)
	//::connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));

	//--------------------------
	while (true)
	{
		char sendBuffer[100] = "Hellow World!";

		//나의 ip주소 + 포트 번호 설정(아무포트 자동으로설정)

		//UnconnectedUDP 기본적인방식
		int32 resultCode = ::sendto(clientSocket, sendBuffer, sizeof(sendBuffer), 0, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
		//Connected UDP tcp처럼 사용(실제로연결된건아님)
		//int32 resultCode = ::send(clientSocket, sendBuffer, sizeof(sendBuffer), 0);

		if (resultCode == SOCKET_ERROR)
		{
			HandleError("SendTo");
			return 0;
		}

		cout << "Send Data Len = " << sizeof(sendBuffer) << endl;

		SOCKADDR_IN recvAddr;
		::memset(&recvAddr, 0, sizeof(recvAddr));
		int32 addrLen = sizeof(recvAddr);

		char recvBuffer[1000];
		//UnconnectedUDP 기본적인방식
		int32 recvLen = ::recvfrom(clientSocket, recvBuffer, sizeof(recvBuffer), 0, (SOCKADDR*)&recvAddr, &addrLen);
		//Connected UDP tcp처럼 사용(실제로연결된건아님)
		//int32 recvLen = ::recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);


		if (recvLen <= 0)
		{
			HandleError("Recvfrom");
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
