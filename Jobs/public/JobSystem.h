#pragma once

#include "CompositeValue.h"
#include "JobSystemDef.h"

#include <queue>
#include <list>

#include <mutex>

namespace jobs
{
	class Job;
	class JobSystem;
	class Thread;

	class JobSystem : public ObjectValue
	{
		friend class Thread;
	private:
		std::queue<Job*> m_jobQueue;
		std::list<Thread*> m_threads;
		std::mutex m_mutex;
		std::mutex m_bootMutex;

		jobs::Job* AcquireJob();
		void BootThread();
	public:
		Value m_numThreads;

		JobSystem(const JobSystemDef& def, CompositeValue* outer);
		virtual ~JobSystem();

		void Start();

		void ScheduleJob(Job* job);
	};
}