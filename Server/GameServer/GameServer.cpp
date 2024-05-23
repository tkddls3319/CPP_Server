﻿#include "pch.h"
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

	SOCKET serverSocket = ::socket(AF_INET, SOCK_DGRAM, 0);

	if (serverSocket == INVALID_SOCKET)
	{
		HandleError("Socket");
	}

	//SO_KEEPALIVE = 주기적으로 연결 상태 확인 여부 (TCP Only), 상대방이 갑자기 연결을 끊는다면 주기적으로 tcp 프로토콜 연결 상태확인
	bool enable = true;
	::setsockopt(serverSocket, SOL_SOCKET, SO_KEEPALIVE, (char*)&enable, sizeof(enable));

	//SO_LINGER =  지연하다. send도중 closesocket를 하여 소켓 리소스 반환을 한다면 송신 버퍼에 있는 데이터를 보낼것인가 날릴것인가
	//l_oneff = 0이면 closesocket()이 바로 리턴 아니면 linger 초만큼 대기 (default 0)
	//l_linger = 대기시간
	LINGER linger;
	linger.l_onoff = 1;
	linger.l_linger = 5;
	::setsockopt(serverSocket, SOL_SOCKET, SO_LINGER, (char*)&linger, sizeof(linger));

	//Half-Close
	//SD_SEND : send 만 막는다
	//SD_RECEIVE : recv 만 막는다.
	//SD_BOATH : 둘다 막는다.
	//::shutdown(serverSocket, SD_SEND);
	//소켓 리소스 반환, SEND하는 도중에 닫아버릴 수 있다. SHUTDOWND과 함께 나와줘야 좋음.
	//::closesocket(serverSocket);

	//SO_SNDBUF = 송신 버퍼 크기
	//SO_RCVBUF = 수신 버퍼 크기
	int32 sendBufferSize;
	int32 optionLen = sizeof(sendBufferSize);
	::getsockopt(serverSocket, SOL_SOCKET, SO_SNDBUF, (char*)&sendBufferSize, &optionLen);
	cout << "송신 버퍼 크기 : " << sendBufferSize << endl;
	int32 recvBufferSize;
	 optionLen = sizeof(recvBufferSize);
	::getsockopt(serverSocket, SOL_SOCKET, SO_RCVBUF, (char*)&recvBufferSize, &optionLen);
	cout << "수신 버퍼 크기 : " << recvBufferSize << endl;

	//SO_REUSEADDR
	//IP주소 및 PORT 재사용 - 서버 껐다켰을 때 재사용 설정안해주면 3분정도 뒤에 다시 사용가능하기 때문에 설정해줘야함.
	{
		bool enable = true;
		::setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&enable, sizeof(enable));
	}

	//IPPROTO_TCP
	//TCP_NODELAY = Nagle 네이글 알고리즘 작동을 끈다.
	//데이터가 충분히 크면 보내고 그렇지 않으면 데이터가 충분히 쌓일때까지 대기.
	//장점 : 작은 패킷이 불필요하게 많이 생성되는 일 방지
	//단점 : 반응 시간 손해
	{
		bool enable = true;
		::setsockopt(serverSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&enable, sizeof(enable));
	}

	//윈도우 소켓 종료 WSADATA호출 수만큼 정리해줘야함(권고사항)
	::WSACleanup();
}