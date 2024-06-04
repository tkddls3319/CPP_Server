#include "pch.h"
#include "JobQueue.h"
#include "GlobalQueue.h"
/*---------------
	  JobQeueu
---------------*/

void JobQueue::Push(JobRef&& job)
{
	const int32 prevCount = _jobCount.fetch_add(1);
	_jobs.Push(job);// WRITE_LOCK

	//ù��° JOB�� ���� �����尡 ������� ���
	if (prevCount == 0)
	{
		//�̹� �������� jobqueue�� ������ ����
		if (LCurrentJobQueue == nullptr)
		{
			Execute();
		}
		else 
		{
			//���� �ִ� �ٸ� �����尡 �����ϵ��� Globalqueue�� �ѱ��.
			GGlobalQueue->Push(shared_from_this());
		}
	}
}

//1)�ϰ��� �ʹ� ������ ���ú� �ȵ�.
//2) doasync Ÿ�� Ÿ���� �� �� ������ �ʴ� ��Ȳ
void JobQueue::Execute()
{
	LCurrentJobQueue = this;

	while (true)
	{
		Vector<JobRef> jobs;
		_jobs.PopAll(OUT jobs);

		const int32 jobCount = static_cast<int32>(jobs.size());
		for (int32 i = 0; i < jobCount; i++)
			jobs[i]->Execute();

		//���� �ϰ��� 0����� ����
		if (_jobCount.fetch_sub(jobCount) == jobCount)
		{
			return;
		}

		const uint64 now = ::GetTickCount64();
		if (now >= LEndTickCount)
		{
			LCurrentJobQueue = nullptr;
			//���� �ִ� �ٸ� �����尡 �����ϵ��� Globalqueue�� �ѱ��.
			GGlobalQueue->Push(shared_from_this());
		}

	}
}
