#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "DeadLockProfiler.h"
#include "SocketUtils.h"

ThreadManager*		GthreadManager = nullptr;
Memory*				GMemory = nullptr;
DeadLockProfiler*	GDeadLockProfiler = nullptr;

class CoreGlobal
{
public:

	CoreGlobal() {
		GthreadManager = new ThreadManager();
		GMemory = new Memory();
		GDeadLockProfiler = new DeadLockProfiler();

		SocketUtils::Init();
	}

	~CoreGlobal() {
		delete GthreadManager;
		delete GMemory;
		delete GDeadLockProfiler;

		SocketUtils::Clear();
	}
}GCoreGlobal; // �ھ�۷ι� ����Ŭ���� �ٷ� ����


