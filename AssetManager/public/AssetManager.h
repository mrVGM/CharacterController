#pragma once

#include "Value.h"
#include "Job.h"

#include <string>

namespace assets
{
	void Boot(jobs::Job* done);
	void Shutdown();
}