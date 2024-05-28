#include "pch.h"
#include "IocpCore.h"
#include "IocpEvent.h"

//temp
IocpCore GlocpCore;

//iocp 생성
IocpCore::IocpCore()
{
	_iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);

	ASSERT_CRASH(_iocpHandle != INVALID_HANDLE_VALUE);
}

IocpCore::~IocpCore()
{
	::CloseHandle(_iocpHandle);
}

//관찰해달라고 요청함수
bool IocpCore::Register(IocpObject* iocpObject)
{
	return ::CreateIoCompletionPort(iocpObject->GetHandle(), _iocpHandle, /*key*/reinterpret_cast<ULONG_PTR>(iocpObject), 0);
}

//일감이있는지 체크
bool IocpCore::Dispatch(uint32 timeoutMs)
{
	DWORD numOfBytes = 0;
	IocpObject* iocpObject = nullptr;
	IocpEvent* iocpEvent = nullptr;

	if (::GetQueuedCompletionStatus(_iocpHandle, OUT & numOfBytes, OUT reinterpret_cast<PULONG_PTR>(&iocpObject), OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), timeoutMs))
	{
		iocpObject->Dispatch(iocpEvent, numOfBytes);//일감이있다면 실행
	}
	else
	{
		int32 errCode = ::WSAGetLastError();

		switch (errCode)
		{
		case WAIT_TIMEOUT://큐값이 없으면 탈 수 있음 정상적임
			return false;
		default:
			//TODO : 로그 찍기 
			break;
		}
	}
	return false;
}
