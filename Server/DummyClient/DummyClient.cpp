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

	//소켓 리소스 반환
	::closesocket(clientSocket);

	//윈도우 소켓 종료 WSADATA호출 수만큼 정리해줘야함(권고사항)
	::WSACleanup();
}
