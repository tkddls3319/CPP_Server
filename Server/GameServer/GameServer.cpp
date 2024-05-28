﻿#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <atomic>
#include <mutex>
#include <windows.h>
#include <future>
#include "ThreadManager.h"

#include "Service.h"
#include "Session.h"

class GameSession : public Session
{

};

int main()
{
	ServerServiceRef service = MakeShared<ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<GameSession>(),
		100);

	ASSERT_CRASH(service->Start());


	for (int32 i = 0; i < 5; i++)
	{
		GthreadManager->Launch([=]()
			{
				while (true)
				{
					service->GetIocpCore()->Dispatch();
				}
			});
	}

	cout << "clineet connect" << endl;

	GthreadManager->Join();
}