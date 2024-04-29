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
	//static void* operator new (size_t size)
	//{
	//	cout << "new" << endl;
	//	void* ptr = ::malloc(size);

	//	return ptr;
	//}
	//static void operator delete(void* ptr)
	//{
	//	cout << "delete" << endl;

	//	::free(ptr);
	//}
};

int main()
{
	Knight* knight = xnew<Knight>();
	xdelete(knight);
}