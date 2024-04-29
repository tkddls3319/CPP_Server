#include "pch.h"
#include "PlayerManager.h"
#include "AccountManager.h"

PlayerManager GPlayerManager;

void PlayerManager::PlayerThenAccount()
{
	WRITE_LOCK;
	GPlayerManager.Lock();
}

void PlayerManager::Lock()
{
	WRITE_LOCK;
}
