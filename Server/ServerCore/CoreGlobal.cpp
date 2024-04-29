#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "DeadLockProfiler.h"

ThreadManager* GthreadManager = nullptr;
DeadLockProfiler* GDeadLockProfiler = nullptr;

class CoreGlobal
{
public:

	CoreGlobal() {
		GthreadManager = new ThreadManager();
		GDeadLockProfiler = new DeadLockProfiler();
	}

	~CoreGlobal() {
		delete GthreadManager;
		delete GDeadLockProfiler;
	}
}GCoreGlobal; // �ھ�۷ι� ����Ŭ���� �ٷ� ����


