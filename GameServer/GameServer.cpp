#include "pch.h"
#include <iostream>
#include <thread>
#include <mutex>
#include<atomic>
#include "ThreadManager.h"
#define OUT

CoreGlobal Core;

void ThreadMain() {
	while (true)
	{
		cout << "쓰레드 id" << LThreadId << endl;
	}
}
int main()
{
	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch(ThreadMain);
	}
	GThreadManager->Join();
}

