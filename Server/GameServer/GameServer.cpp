#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <thread>
#include <future>
#include "CoreMacro.h"
#include "ThreadManager.h"

CoreGlobal Core;

void ThreadMain() {
	while (true)
	{
		cout << "번스레드" << LThreadId << endl;

		this_thread::sleep_for(1s);
	}
}

int main()
{
	GthreadManager->Launch(ThreadMain);
	GthreadManager->Join();
}