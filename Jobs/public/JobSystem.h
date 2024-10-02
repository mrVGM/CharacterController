#pragma once

#include "CompositeValue.h"
#include "JobSystemDef.h"

#include "Channel.h"
#include "Jobs.h"

#include <list>

namespace jobs
{
	class JobSystem;
	class Thread;

	class JobSystem : public ObjectValue
	{
		friend class Thread;
	private:
		Channel<Job> m_jobsChannel;
		std::list<Thread*> m_threads;

		jobs::Job AcquireJob();
	public:
		Value m_numThreads;

		JobSystem(const ReferenceTypeDef& def);
		virtual ~JobSystem();

		void Start();

		void ScheduleJob(const Job& job);
	};
}