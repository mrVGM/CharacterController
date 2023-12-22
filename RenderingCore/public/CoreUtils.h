#pragma once

#include "RenderWindow.h"
#include "DXDevice.h"
#include "DXSwapChain.h"

#include <d3d12.h>

namespace rendering::core::utils
{
	WindowObj* GetWindow();
	DXDevice* GetDevice();
	DXSwapChain* GetSwapChain();
}