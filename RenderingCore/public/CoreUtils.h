#pragma once

#include "RenderWindow.h"
#include "DXDevice.h"
#include "DXSwapChain.h"
#include "DXCommandQueue.h"
#include "DXFence.h"
#include "JobSystem.h"
#include "DXCopyBuffers.h"
#include "DXTexture.h"

namespace rendering::core::utils
{
	void Get3DMaterialInputLayout(const D3D12_INPUT_ELEMENT_DESC*& layout, unsigned int& numElements);
	void Get3DSkeletalMeshMaterialInputLayout(const D3D12_INPUT_ELEMENT_DESC*& layout, unsigned int& numElements);

	WindowObj* GetWindow();
	DXDevice* GetDevice();
	DXCommandQueue* GetCommandQueue();
	DXSwapChain* GetSwapChain();
	DXFence* GetRenderFence();
	DXFence* GetResidentHeapFence();
	jobs::JobSystem* GetResidentHeapJobSystem();
	DXCopyBuffers* GetCopyBuffers();
	DXTexture* GetDepthStencilTexture();
}