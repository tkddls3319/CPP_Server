#include "pch.h"
#include <iostream>
#include <thread>
#include <future>
#include "ThreadManager.h"

#include "PlayerManager.h"
#include "AccountManager.h"


int main()
{
	GthreadManager->Launch([=]
		{
			while (true)
			{
				cout << "PlayerThenAccount" << endl;
				GPlayerManager.PlayerThenAccount();
				this_thread::sleep_for(1ms);
			}
		});


	GthreadManager->Launch([=]
		{
			while (true)
			{
				cout << "AccountThenPlayer" << endl;
				GAccountManager.AccountThenPlayer();
				this_thread::sleep_for(1ms);
			}
		});
	GthreadManager->Join();
}