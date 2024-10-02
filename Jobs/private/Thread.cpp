#include "Thread.h"

#include "JobSystem.h"
#include "Channel.h"
#include "Jobs.h"


namespace
{
	int run_thrd(void* arg)
	{
		using namespace jobs;

		Thread* thread = static_cast<Thread*>(arg);

		while (!thread->ShouldStop())
		{
			Job job = thread->GetJob();
			job();
		}
		return 0;
	}
}

jobs::Thread::Thread(JobSystem& jobSystem) :
	m_jobSystem(jobSystem)
{
	Start();
}

jobs::Thread::~Thread()
{
	Stop();
	m_thread->join();
	delete m_thread;
}

void jobs::Thread::Start()
{
	thrd_create(&m_thrd, run_thrd, this);
}

void jobs::Thread::Stop()
{
	m_stopped = true;
}

bool jobs::Thread::ShouldStop()
{
	return m_stopped;
}

jobs::Job jobs::Thread::GetJob()
{
	return m_jobSystem.AcquireJob();
}

