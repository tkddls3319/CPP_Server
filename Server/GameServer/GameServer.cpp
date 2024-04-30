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
#include "Allocator.h"

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
	Vector<Knight> v(100);

}