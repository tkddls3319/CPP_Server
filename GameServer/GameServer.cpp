#include "pch.h"
#include <iostream>
#include <thread>
#include <mutex>
#include<atomic>
#include "ThreadManager.h"
#define OUT
#include "PlayerManager.h"
#include "AccountManager.h"

#include "RefCounting.h"
#include "Memory.h"

using KnightRef = TSharedPtr<class Knight>;
class Knight : public RefCountable
{
public:
	void SetTarget(KnightRef target)
	{
		_target = target;
	}
	KnightRef _target = nullptr;
};


int main()
{
	Knight* knight = xnew<Knight>();

	xdelete(knight);
}

