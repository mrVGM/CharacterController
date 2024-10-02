#include "JobSystem.h"

#include "PrimitiveTypes.h"

#include "Thread.h"
#include "Jobs.h"

jobs::JobSystem::JobSystem(const ReferenceTypeDef& def) :
	ObjectValue(def),
	m_numThreads(IntTypeDef::GetTypeDef(), this)
{
}

jobs::JobSystem::~JobSystem()
{
	for (auto it = m_threads.begin(); it != m_threads.end(); ++it)
	{
		delete (*it);
	}
}

void jobs::JobSystem::Start()
{
	int numThreads = m_numThreads.Get<int>();
	if (m_threads.size() == numThreads)
	{
		return;
	}

	for (int i = 0; i < numThreads; ++i)
	{
		m_threads.push_back(new Thread(*this));
	}
}


void jobs::JobSystem::ScheduleJob(const Job& job)
{
	m_jobsChannel.Push(job);
}

jobs::Job jobs::JobSystem::AcquireJob()
{
	Job job = m_jobsChannel.Pop();
	return job;
}