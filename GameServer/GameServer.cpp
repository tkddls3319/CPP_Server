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


class Wraight : public RefCountable
{
public:
	int _hp = 150;
	int _posX = 0;
	int _posY = 0;
};
using WraightRef = TSharedPtr<Wraight>;
class Missile : public RefCountable
{
public:
	void SetTarget(WraightRef target)
	{
		_target = target;
	}
	bool Update()
	{
		if (_target == nullptr)
			return true;

		int posX = _target->_posX;
		int posY = _target->_posY;

		//TODO : 쫓아가기

		if (_target->_hp == 0)
		{
			_target = nullptr;
			return true;
		}

		return false;
	}

	WraightRef _target = nullptr;
};


using MissileRef = TSharedPtr<Missile>;

int main()
{
	WraightRef wraight(new Wraight());
	wraight->ReleaseRef();
	MissileRef missile(new Missile());
	missile->ReleaseRef();

	missile->SetTarget(wraight);

	//레이스 피격 당함
	wraight->_hp = 0;
	wraight = nullptr;

	while (true)
	{
		if (missile) 
		{
			if (missile->Update())
			{
				missile = nullptr;
			}
		}
	}
	wraight = nullptr;
}

