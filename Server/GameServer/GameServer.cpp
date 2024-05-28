#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <atomic>
#include <mutex>
#include <windows.h>
#include <future>
#include "ThreadManager.h"

#include "SocketUtils.h"
#include "Listener.h"

int main()
{
	//SOCKET socket = SocketUtils::CreateSocket();
	//SocketUtils::BindAnyAddress(socket, 7777);
	//SocketUtils::Listen(socket);
	//::accept(socket, nullptr, nullptr);

	Listener listener;
	listener.StartAccept(NetAddress(L"127.0.0.1", 7777));

	for (int32 i = 0; i < 5; i++)
	{
		GthreadManager->Launch([=]()
			{
				while (true)
				{
					GlocpCore.Dispatch();
				}
			});
	}

	cout << "clineet connect" << endl;

	GthreadManager->Join();
}