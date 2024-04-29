#pragma once
#include "Types.h"

/*------------
read write SpinLock
-------------*/

/*-------------------------
W : WriteFlag (Exclusive lock owner threadId) 16��Ʈ
R : ReadFlag (Shared Lock Count) 16��Ʈ
--------------------------*/

/*��Ģ
 W -> R (0)
 W -> W (0)
 R -> W (X)*/
class Lock
{
	enum : uint32 {
		ACQUIRE_TIMEOUT_TICK = 100000,//�ִ� ��ٸ� �ð�
		MAX_SPIN_COUNT = 5000,//��� �õ��� �� �ΰ�
		WRITE_THREAD_MASK = 0XFFFF'0000,//W ���� 16��Ʈ �� 1
		READ_COUNT_MASK = 0X0000'FFFF,//R ���� 16��Ʈ �� 1
		EMPTY_FLAG = 0X0000'0000 //�ʱⰪ
	};
public:
	void WriteLock(const char* name);
	void WriteUnLock(const char* name);
	void ReadLock(const char* name);
	void ReadUnLock(const char* name);

private:
	Atomic<uint32> _lockFlag;
	uint16 _writeCount = 0;
};

/*-----------------
	LockGuards
-------------------*/
class ReadLockGuard {
public:
	ReadLockGuard(Lock& lock, const char* name) : _lock(lock), _name(name)
	{
		_lock.ReadLock(_name);
	}
	~ReadLockGuard()
	{
		_lock.ReadUnLock(_name);
	}
private:
	Lock& _lock;
	const char* _name;
};

class WriteLockGuard {
public:
	WriteLockGuard(Lock& lock, const char* name) : _lock(lock), _name(name)
	{
		_lock.WriteLock(_name);
	}
	~WriteLockGuard()
	{
		_lock.WriteUnLock(_name);
	}
private:
	Lock& _lock;
	const char* _name;
};