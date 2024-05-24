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
	WSAOVERLAPPED overlapped = {};
	SOCKET socket;
	char recvBuffer[BUFSIZE] = {};
	int32 recvBytes = 0;
};

void CALLBACK RecvCallBack(DWORD error, DWORD recvLen, LPWSAOVERLAPPED overlapped, DWORD flags)
{
	cout << "Data recv Len" << recvLen << endl;
	//TODO : 에코 서버를 만든다 WSASend();

	Session* session = (Session*)overlapped;
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

	//Overlapped (콜백기반)
	//비동기 입출력 지원하는 소켓 생성
	//비동기 입출력 함수 호출한다다(완료 루틴의 시작 주소를 넘겨준다.)
	//비동기 방식이기 때문에 바로 실행되지 않을 수 있다. 바로 완료되지 않으면 WSA_IO_PENDING오류가 뜬다.
	// 비동기 입출력 함수 호출한 쓰레드를 Alertable Wait 상태로 만든다.
	// ex) WaitForSingleObjectEx, WaitForMultipleObjectsEx, SleepEx, WSAWAitForMultipleEvents
	//비동기 io완료되면 운영체제는 완료 루틴 호출
	//완료 루틴 호출이 모두 끝나면 쓰레드는 Alertable Wait 상태에서 빠져나온다.

	//void CompletionRoutine()
	//1) 오류 발생시 0 아닌 값
	//2) 전송 바이트수
	//3) 비도익 입출력 함수 호출 시 넘겨준 WSAOVERLAPPED 구조체의 주소값
	//4) 0

	//select 모델
	//장점) 윈도우/리눅스 공통
	//단점) 서능 최하 매번등록해야하는 비용, 64개로 제한
	//WSAEventSelect 모델
	//장점) 비교적 뛰어난 성능(클라이언트에 적합)
	//단점) 64개로 제한
	//Overlapped (이벤트 기반)
	//장점) 성능이 좋다
	//단점) 64개 제한
	//Overlapped (콜백 기반)
	//장점) 성능이 좋다
	//단점) 모든 비동기 소켓 함수에서 사용 가능하진 않다(accept)

	while (true)
	{
		SOCKADDR_IN clientAddr;
		int32 addrLen = sizeof(clientAddr);

		SOCKET clientSocket;
		while (true)
		{
			clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
			if (clientSocket != INVALID_SOCKET)
				break;

			if (::WSAGetLastError() == WSAEWOULDBLOCK)
				continue;

			return 0;
		}

		Session session = Session{ clientSocket };
		cout << "client connected" << endl;

		while (true)
		{
			WSABUF wsaBuf;
			wsaBuf.buf = session.recvBuffer;
			wsaBuf.len = BUFSIZE;

			DWORD recvLen = 0;
			DWORD flags = 0;
			if (::WSARecv(clientSocket, &wsaBuf, 1, &recvLen, &flags, &session.overlapped, RecvCallBack) == SOCKET_ERROR)
			{
				if (::WSAGetLastError() == WSA_IO_PENDING)
				{
					//pending  지연
					::SleepEx(INFINITE, TRUE);//Alertable Wait apc 상태
				}
				else
				{
					//TODO : 문제 상황
				}
			}
			else
			{
				cout << "Data Recv Len = " << recvLen << endl;
			}
		}
		::closesocket(session.socket);
	}
	::WSACleanup();
}