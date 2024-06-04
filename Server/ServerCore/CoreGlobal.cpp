#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "DeadLockProfiler.h"
#include "SocketUtils.h"
#include "SendBuffer.h"
#include "GlobalQueue.h"

ThreadManager*		GThreadManager = nullptr;
Memory*				GMemory = nullptr;
SendBufferManager*  GSendBufferManager = nullptr;
GlobalQueue*		GGlobalQueue = nullptr;
DeadLockProfiler*	GDeadLockProfiler = nullptr;

class CoreGlobal
{
public:

	CoreGlobal() {
		GThreadManager = new ThreadManager();
		GMemory = new Memory();
		GSendBufferManager = new SendBufferManager();
		GDeadLockProfiler = new DeadLockProfiler();
		GGlobalQueue = new GlobalQueue();
		SocketUtils::Init();
	}

	~CoreGlobal() {
		delete GThreadManager;
		delete GMemory;
		delete GSendBufferManager;
		delete GGlobalQueue;
		delete GDeadLockProfiler;
		SocketUtils::Clear();
	}
}GCoreGlobal; // 코어글로벌 전역클래스 바로 선언


