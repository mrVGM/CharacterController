#pragma once

#include "RenderWindow.h"
#include "DXDevice.h"
#include "DXSwapChain.h"
#include "DXCommandQueue.h"
#include "DXFence.h"
#include "JobSystem.h"
#include "DXCopyBuffers.h"

namespace rendering::core::utils
{
	WindowObj* GetWindow();
	DXDevice* GetDevice();
	DXCommandQueue* GetCommandQueue();
	DXSwapChain* GetSwapChain();
	DXFence* GetRenderFence();
	DXFence* GetResidentHeapFence();
	jobs::JobSystem* GetResidentHeapJobSystem();
	DXCopyBuffers* GetCopyBuffers();
}