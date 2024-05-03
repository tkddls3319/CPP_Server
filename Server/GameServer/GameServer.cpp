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

class Player 
{
	Player();
};
class Mage :public Player
{
public:
	Mage()
	{
		cout << "k" << endl;
	}

private:
	int32 _hp = 100;
};

class Knight :public Player 
{
public:
	Knight()
	{
		cout << "k" << endl;
	}
private:
	int32 _hp = 100;
};
class Archer :public Player
{
public:
	Archer()
	{
		cout << "k" << endl;
	}
private:
	int32 _hp = 100;
};

template<typename T, typename U>
struct TypeList
{
	using Head = T;
	using Tail = U;
};

#include "TypeCast.h"
int main()
{
	TypeList<Mage, Knight>::Head whoAmi;
	TypeList<Mage, Knight>::Tail whoAmi;

	TypeList<Mage, TypeList<Knight, Archer>>::Tail whoAmi;

	for (int32 i = 0; i < 5; i++)
	{
		GthreadManager->Launch([]() {

			while (true)
			{
				Vector<Knight> v(10);
				Map<int32, Knight> m;
				m[100] = Knight();

				this_thread::sleep_for(10ms);
			}
			});
	}

	GthreadManager->Join();
}