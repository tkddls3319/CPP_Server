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

	//첫번째 JOB을 넣은 쓰레드가 실행까지 담당
	if (prevCount == 0)
	{
		//이미 실행중인 jobqueue가 없으면 실행
		if (LCurrentJobQueue == nullptr)
		{
			Execute();
		}
		else 
		{
			//여유 있는 다른 쓰레드가 실행하도록 Globalqueue에 넘긴다.
			GGlobalQueue->Push(shared_from_this());
		}
	}
}

//1)일감이 너무 몰리면 리시브 안됨.
//2) doasync 타고 타고가서 절 대 끝나지 않는 상황
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

		//남은 일감이 0개라면 종료
		if (_jobCount.fetch_sub(jobCount) == jobCount)
		{
			return;
		}

		const uint64 now = ::GetTickCount64();
		if (now >= LEndTickCount)
		{
			LCurrentJobQueue = nullptr;
			//여유 있는 다른 쓰레드가 실행하도록 Globalqueue에 넘긴다.
			GGlobalQueue->Push(shared_from_this());
		}

	}
}
