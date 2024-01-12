#include "JobSystem.h"

#include "PrimitiveTypes.h"

#include "Job.h"
#include "Thread.h"

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

	while (!m_jobQueue.empty())
	{
		Job* cur = m_jobQueue.front();
		m_jobQueue.pop();
		delete cur;
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

void jobs::JobSystem::BootThread()
{
	for (auto it = m_threads.begin(); it != m_threads.end(); ++it)
	{
		if ((*it)->IsBusy())
		{
			continue;
		}

		(*it)->Boot();
		return;
	}
}

void jobs::JobSystem::ScheduleJob(Job* job)
{
	m_mutex.lock();
	m_jobQueue.push(job);
	m_mutex.unlock();

	m_bootMutex.lock();
	BootThread();
	m_bootMutex.unlock();
}

jobs::Job* jobs::JobSystem::AcquireJob()
{
	Job* job = nullptr;
	m_mutex.lock();
	if (!m_jobQueue.empty())
	{
		job = m_jobQueue.front();
		m_jobQueue.pop();
	}
	m_mutex.unlock();

	return job;
}