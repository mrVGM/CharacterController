#include "RenderingCore.h"

#include "RenderWindow.h"
#include "RenderingEntryTypeDef.h"
#include "DXShader.h"
#include "DXDevice.h"
#include "DXSwapChain.h"
#include "DXCommandQueue.h"
#include "DXFence.h"

#include "ResidentHeapFence.h"
#include "ResidentHeapJobSystem.h"

void rendering::core::Boot()
{
	WindowTypeDef::GetTypeDef();
	RenderingEntryTypeDef::GetTypeDef();

	DXShaderTypeDef::GetTypeDef();
	DXVertexShaderTypeDef::GetTypeDef();
	DXPixelShaderTypeDef::GetTypeDef();

	DXDeviceTypeDef::GetTypeDef();
	DXSwapChainTypeDef::GetTypeDef();
	DXCommandQueueTypeDef::GetTypeDef();
	DXFenceTypeDef::GetTypeDef();

	ResidentHeapFenceTypeDef::GetTypeDef();
	ResidentHeapJobSystemTypeDef::GetTypeDef();
}