#pragma once

#include "JobSystem.h"
#include "Job.h"

#include "Value.h"

namespace jobs
{
	void Boot(Value& mainJobSystemAsset, Value& asyncJobSystemAsset);

	void RunSync(Job* job);
	void RunAsync(Job* job);
}