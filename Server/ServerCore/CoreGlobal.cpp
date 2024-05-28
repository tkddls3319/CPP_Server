#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "DeadLockProfiler.h"
#include "SocketUtils.h"

ThreadManager*		GThreadManager = nullptr;
Memory*				GMemory = nullptr;
DeadLockProfiler*	GDeadLockProfiler = nullptr;

class CoreGlobal
{
public:

	CoreGlobal() {
		GThreadManager = new ThreadManager();
		GMemory = new Memory();
		GDeadLockProfiler = new DeadLockProfiler();

		SocketUtils::Init();
	}

	~CoreGlobal() {
		delete GThreadManager;
		delete GMemory;
		delete GDeadLockProfiler;

		SocketUtils::Clear();
	}
}GCoreGlobal; // 코어글로벌 전역클래스 바로 선언


