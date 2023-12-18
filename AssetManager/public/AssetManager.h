#pragma once

#include <string>

#include "Value.h"

namespace assets
{
	void Boot();
	void Shutdown();
	Value& GetAssetList();
}