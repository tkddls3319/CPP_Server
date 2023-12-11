#include "pch.h"
#include "DeadLockProfiler.h"

void DeadLockProfiler::PushLock(const char* name)
{
	LockGuard guard(_lock);

	//아이디를 찾거나 발급한다
	int32 lockId = 0;

	auto findId = _nameToId.find(name);
		if (findId == _nameToId.end())
		{
			lockId = static_cast<int32>(_nameToId.size());
			_nameToId[name] = lockId;
			_idToName[lockId] = name;
		}else
		{
			lockId = findId->second;
		}

		//잡고있는 락이 있었다면
		if (_lockStack.empty() == false)
		{
			//기존에 발견되지 않은 케이스라면 데드락 여부 다시 확인하기.
			const int32 prevId = _lockStack.top();
			if (lockId != prevId)
			{
				set<int32>& history = _lockHistory[prevId];
				if (history.find(lockId) == history.end()) {
					history.insert(lockId);
					CheckCycle();
				}
			}
		}

		_lockStack.push(lockId);

}

void DeadLockProfiler::PopLock(const char* name)
{
	LockGuard guard(_lock);

	if (_lockStack.empty())
		CRASH("MULTIPLE_UNLOCK");

	int32 lockId = _nameToId[name];
	if(_lockStack.top() != lockId)

	_lockStack.pop()
}

void DeadLockProfiler::CheckCycle()
{
	const int32 lockCount = static_cast<int32>(_nameToId.size());
	_discoveredOrder = vector<int32>(lockCount, -1);
	_discoveredCount = 0;
	_finished = vector<bool>(lockCount, false);
}

void DeadLockProfiler::Dfs(int32 index)
{
}
