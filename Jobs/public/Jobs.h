#pragma once

#include "JobSystem.h"
#include "Job.h"

#include "Value.h"

namespace jobs
{
	void Boot();

	void RunSync(Job* job);
	void RunAsync(Job* job);
}