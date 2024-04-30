#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <atomic>
#include <mutex>
#include <windows.h>
#include <future>
#include "ThreadManager.h"

#include "RefCounting.h"
#include "Memory.h"

class Knight {
public:
	Knight()
	{
		cout << "k" << endl;
	}
	~Knight() {

		cout << "~k" << endl;
	}

	int32 _hp = 100;
};

int main()
{
	Knight* knight = xnew<Knight>();
	xdelete(knight);

	knight->_hp = 1000;
}