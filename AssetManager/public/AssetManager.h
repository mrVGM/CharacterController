#pragma once

#include <string>

#include "Value.h"

namespace assets
{
	void Boot(const Value& preloadedAssets);
	void Shutdown();
	Value& GetAssetList();
}