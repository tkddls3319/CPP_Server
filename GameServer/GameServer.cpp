#include "pch.h"
#include <iostream>
#include <thread>
#include <mutex>
#include<atomic>
#include "ThreadManager.h"
#define OUT
#include "PlayerManager.h"
#include "AccountManager.h"


int main()
{
	GThreadManager->Launch([=]
		{
			while (true)
			{
				cout << "PlayerThenAccount" << endl;
				GPlayerManager.PlayerThenAccount();
				this_thread::sleep_for(100ms);
			}
		});
	GThreadManager->Launch([=]
		{
			while (true)
			{
				cout << "AccountThenPlayer" << endl;
				GAccountManager.AccountThenPlayer();
				this_thread::sleep_for(100ms);
			}
		});

	GThreadManager->Join();

}

