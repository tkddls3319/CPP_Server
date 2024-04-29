#pragma once
#include "Types.h"

/*------------
read write SpinLock
-------------*/

/*-------------------------
W : WriteFlag (Exclusive lock owner threadId) 16비트
R : ReadFlag (Shared Lock Count) 16비트
--------------------------*/

/*규칙
 W -> R (0)
 W -> W (0)
 R -> W (X)*/
class Lock
{
	enum : uint32 {
		ACQUIRE_TIMEOUT_TICK = 100000,//최대 기다릴 시간
		MAX_SPIN_COUNT = 5000,//몇번 시도할 것 인가
		WRITE_THREAD_MASK = 0XFFFF'0000,//W 상위 16비트 값 1
		READ_COUNT_MASK = 0X0000'FFFF,//R 하위 16비트 값 1
		EMPTY_FLAG = 0X0000'0000 //초기값
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