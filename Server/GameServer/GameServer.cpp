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

const int32 BUFSIZE = 1000;
struct Session
{
	SOCKET socket;
	char recvBuffer[BUFSIZE] = {};
	int32 recvBytes = 0;
	int32 sendBytes = 0;
};

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

	//fd_set set;
	//FD_ZERO(set) : 초기화(비운다)
	//FD_SET(s, &set) : 소켓 s를 넣는다
	//FD_CLR(s, &set) : 소켓 s를 제거
	//FD_ISSET : 소켓 s가 set에 들어있으면 0이 아닌 값을 리턴한다. 

	vector<Session> sessions;
	sessions.reserve(100);

	fd_set reads;
	fd_set writes;

	while (true)
	{
		//소켓 셋 초기화
		FD_ZERO(&reads);
		FD_ZERO(&writes);

		//ListenSocket 등록
		FD_SET(listenSocket, &reads);

		//소켓 등록
		for (Session& s : sessions)
		{
			if (s.recvBytes <= s.sendBytes) 
				FD_SET(s.socket, &reads);
			else
				FD_SET(s.socket, &writes);
		}

		//옵션 마지막 timeout 인자 설정 가능
	/*	timeval timeout;
		timeout.tv_sec;
		timeout.tv_usec;*/
		int32 retVal = ::select(0, &reads, &writes, nullptr, nullptr);//관찰시작 낙오자 제거
		if (retVal == SOCKET_ERROR)
			break;

		//Listener 소켓 체크
		if (FD_ISSET(listenSocket, &reads))
		{
			SOCKADDR_IN clientAddr;//IPv4
			::memset(&clientAddr, 0, sizeof(clientAddr));
			int32 addrLen = sizeof(clientAddr);
			SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);

			if (clientSocket != INVALID_SOCKET)
			{
				cout << "client connected" << endl;
				sessions.push_back(Session{ clientSocket });
			}
		}

		//나머지 소켓 체크 read나 write
		for (Session& s : sessions )
		{
			//read 체크
			if (FD_ISSET(s.socket, &reads)) 
			{
				int32 recvLen = ::recv(s.socket, s.recvBuffer, BUFSIZE, 0);
				if (recvLen <= 0)
				{
					//TODO : SESSIONS 제거 
					continue;
				}

				s.recvBytes = recvLen;
			}

			//read 체크
			if (FD_ISSET(s.socket, &writes))
			{
				int32 sendLen = ::send(s.socket, &s.recvBuffer[s.sendBytes],s.recvBytes - s.sendBytes, 0);
				if (sendLen == SOCKET_ERROR)
				{
					//TODO : SESSIONS 제거 
					continue;
				}

				s.sendBytes += sendLen;
				if (s.recvBytes == s.sendBytes)
				{
					s.recvBytes = 0;
					s.sendBytes = 0;
				}
			}
		}

	}


	::WSACleanup();
}