#pragma once
#include "JobSeriallizer.h"

class Room : public JobSeriallizer
{
public:
	// �̱۾����� ȯ���θ��� �ڵ�
	void Enter(PlayerRef player);
	void Leave(PlayerRef player);
	void Broadcast(SendBufferRef sendBuffer);

public:
	// ��Ƽ������ ȯ�濡���� �ϰ����� ����
	//void PushJob(JobRef job) { _jobs.Push(job); }
	virtual void FlushJob() override;


private:
	map<uint64, PlayerRef> _players;

	//JobQueue _jobs;
};

extern shared_ptr<Room> GRoom;
