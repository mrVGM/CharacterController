#pragma once

#include "Value.h"
#include "Job.h"

#include <string>

namespace assets
{
	void Boot(const Value& preloadedAssets, jobs::Job* done);
	void Shutdown();
	Value& GetAssetList();
}