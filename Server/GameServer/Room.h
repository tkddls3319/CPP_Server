#pragma once
#include "JobSeriallizer.h"

class Room : public JobSeriallizer
{
public:
	// 싱글쓰레드 환경인마냥 코딩
	void Enter(PlayerRef player);
	void Leave(PlayerRef player);
	void Broadcast(SendBufferRef sendBuffer);

public:
	// 멀티쓰레드 환경에서는 일감으로 접근
	//void PushJob(JobRef job) { _jobs.Push(job); }
	virtual void FlushJob() override;


private:
	map<uint64, PlayerRef> _players;

	//JobQueue _jobs;
};

extern shared_ptr<Room> GRoom;
