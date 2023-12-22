#pragma once

#include "RenderWindow.h"
#include "DXDevice.h"
#include "DXSwapChain.h"
#include "DXCommandQueue.h"

#include <d3d12.h>

namespace rendering::core::utils
{
	WindowObj* GetWindow();
	DXDevice* GetDevice();
	DXCommandQueue* GetCommandQueue();
	DXSwapChain* GetSwapChain();
}