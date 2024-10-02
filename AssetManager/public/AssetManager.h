#pragma once

#include "Value.h"
#include "Jobs.h"

#include <string>

namespace assets
{
	void Boot(jobs::Job done);
	void Shutdown();
}