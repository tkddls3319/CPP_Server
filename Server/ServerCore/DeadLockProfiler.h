#pragma once
#include <stack>
#include <map>
#include <vector>
/*------------------
 DeadLock Profiler
------------------*/
class DeadLockProfiler
{
public:
	void PushLock(const char* name);
	void PopLock(const char* name);
	void CheckCycle();
private:
	void Dfs(int32 index);

private:
	unordered_map<const char*, int32 > _nameToId;
	unordered_map<int32, const char* > _idToName;
	stack<int32> _lockStack;
	map<int32, set<int32>> _lockHistory;

	Mutex _lock;

private:
	vector<int32> _discoveredOrder;//��尡 �߰ߵ� ������ ����ϴ� �迭
	int32 _discoveredCount = 0;//��尡 �߰ߵ� ����
	vector<bool> _finished;//(dfs)�� ����Ǿ����� ����
	vector<int32> _parent;
};

