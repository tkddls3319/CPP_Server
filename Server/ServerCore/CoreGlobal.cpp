#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"


ThreadManager* GthreadManager = nullptr;

CoreGlobal::CoreGlobal()
{
	GthreadManager = new ThreadManager();
}

CoreGlobal::~CoreGlobal()
{
	delete GthreadManager;
}
