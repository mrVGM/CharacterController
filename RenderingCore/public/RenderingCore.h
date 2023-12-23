#pragma once

#include "Jobs.h"

namespace rendering::core
{
	void Boot();

	void LoadCoreObjects(jobs::Job* done);
}