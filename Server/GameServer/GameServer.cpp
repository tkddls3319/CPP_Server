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
};

enum IO_TYPE
{
	READ,
	WRITE,
	ACCEPT,
	CONNECT,
};

struct OverlappedEx
{
	WSAOVERLAPPED overlapped = {};
	int32 type = 0;
};

void WorkerThreadMain(HANDLE iocpHandle)
{
	while (true)
	{
		DWORD bytesTransferred = 0;
		Session* session = nullptr;
		OverlappedEx* overlappedEx = nullptr;

		//이쪽에서 대기 성공하면 밑으로 내려감.
		BOOL ret = ::GetQueuedCompletionStatus(iocpHandle, &bytesTransferred, (ULONG_PTR*)&session, (LPOVERLAPPED*)&overlappedEx, INFINITE);

		if (ret = FALSE || bytesTransferred == 0)
		{
			//TODO : 연결끊김
			continue;
		}

		ASSERT_CRASH(overlappedEx->type == IO_TYPE::READ);
		cout << "Recv Data IOCP =" << bytesTransferred << endl;

		WSABUF wsaBuf;
		wsaBuf.buf = session->recvBuffer;
		wsaBuf.len = BUFSIZE;

		DWORD recvLen = 0;
		DWORD flags = 0;
		//다시 RECV 예약!
		::WSARecv(session->socket, &wsaBuf, 1, &recvLen, &flags, &overlappedEx->overlapped, NULL);
	}
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

	vector<Session*> sessionManager;

	//CompletionPort 생성
	HANDLE iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	//WorkerThreads 생성 CompletionPort를 관찰하면서 완료된 입출력 함수를 처리한다.(recv 전용)
	for (int32 i = 0; i < 5; i++)
	{
		GthreadManager->Launch([=]()
			{
				WorkerThreadMain(iocpHandle);
			});
	}

	//Main Thread = Accept 담당
	while (true)
	{
		SOCKADDR_IN clientAddr;
		int32 addrLen = sizeof(clientAddr);

		SOCKET	clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
		if (clientSocket == INVALID_SOCKET)
			return 0;

		Session* session = new Session();
		session->socket = clientSocket;
		sessionManager.push_back(session);

		cout << "client connected" << endl;

		//소켓을 cp에 등록 관찰시작
		::CreateIoCompletionPort((HANDLE)clientSocket, iocpHandle,  /*key*/(ULONG_PTR)session, 0);

		WSABUF wsaBuf;
		wsaBuf.buf = session->recvBuffer;
		wsaBuf.len = BUFSIZE;

		OverlappedEx* overlappedEx = new OverlappedEx();
		overlappedEx->type = IO_TYPE::READ;

		DWORD recvLen = 0;
		DWORD flags = 0;
		//메인스레드에서 한번 호출 하고 다른 스레드에서 관리. 메인스레드는 다시 accept호출 하로감.
		::WSARecv(clientSocket, &wsaBuf, 1, &recvLen, &flags, &overlappedEx->overlapped, NULL);

	}

	GthreadManager->Join();
	::WSACleanup();
}