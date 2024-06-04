#pragma once
#include "JobQueue.h"

class Room : public JobQueue
{
public:
	// �̱۾����� ȯ���θ��� �ڵ�
	void Enter(PlayerRef player);
	void Leave(PlayerRef player);
	void Broadcast(SendBufferRef sendBuffer);

public:


private:
	map<uint64, PlayerRef> _players;

	//JobQueue _jobs;
};

extern shared_ptr<Room> GRoom;
