#pragma once

#include "Jobs.h"

#include <threads.h>

namespace jobs
{
	class JobSystem;

	class Thread
	{
	private:
		thrd_t m_thrd;

		bool m_stopped = false;
		jobs::JobSystem& m_jobSystem;
	public:
		Thread(JobSystem& jobSystem);
		~Thread();

		void Start();
		void Stop();
		bool ShouldStop();

		jobs::Job GetJob();
	};
}