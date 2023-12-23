#include "CoreUtils.h"

#include "RenderFence.h"
#include "ResidentHeapFence.h"
#include "ResidentHeapJobSystem.h"

#include "ObjectValueContainer.h"

rendering::WindowObj* rendering::core::utils::GetWindow()
{
	std::list<ObjectValue*> tmp;
	ObjectValueContainer& container = ObjectValueContainer::GetContainer();

	container.GetObjectsOfType(WindowTypeDef::GetTypeDef(), tmp);
	WindowObj* wnd = static_cast<WindowObj*>(tmp.front());
	return wnd;
}

rendering::DXDevice* rendering::core::utils::GetDevice()
{
	std::list<ObjectValue*> tmp;
	ObjectValueContainer& container = ObjectValueContainer::GetContainer();

	container.GetObjectsOfType(DXDeviceTypeDef::GetTypeDef(), tmp);
	DXDevice* device = static_cast<DXDevice*>(tmp.front());
	return device;
}

rendering::DXCommandQueue* rendering::core::utils::GetCommandQueue()
{
	std::list<ObjectValue*> tmp;
	ObjectValueContainer& container = ObjectValueContainer::GetContainer();

	container.GetObjectsOfType(DXCommandQueueTypeDef::GetTypeDef(), tmp);
	DXCommandQueue* commandQueue = static_cast<DXCommandQueue*>(tmp.front());
	return commandQueue;
}

rendering::DXSwapChain* rendering::core::utils::GetSwapChain()
{
	std::list<ObjectValue*> tmp;
	ObjectValueContainer& container = ObjectValueContainer::GetContainer();

	container.GetObjectsOfType(DXSwapChainTypeDef::GetTypeDef(), tmp);
	DXSwapChain* swapChain = static_cast<DXSwapChain*>(tmp.front());
	return swapChain;
}

rendering::DXFence* rendering::core::utils::GetRenderFence()
{
	ObjectValue* obj = ObjectValueContainer::GetObjectOfType(RenderFenceTypeDef::GetTypeDef());
	DXFence* fence = static_cast<DXFence*>(obj);
	return fence;
}

rendering::DXFence* rendering::core::utils::GetResidentHeapFence()
{
	ObjectValue* obj = ObjectValueContainer::GetObjectOfType(ResidentHeapFenceTypeDef::GetTypeDef());
	DXFence* fence = static_cast<DXFence*>(obj);
	return fence;
}

jobs::JobSystem* rendering::core::utils::GetResidentHeapJobSystem()
{
	ObjectValue* obj = ObjectValueContainer::GetObjectOfType(ResidentHeapJobSystemTypeDef::GetTypeDef());
	jobs::JobSystem* js = static_cast<jobs::JobSystem*>(obj);
	return js;
}

rendering::DXCopyBuffers* rendering::core::utils::GetCopyBuffers()
{
	ObjectValue* obj = ObjectValueContainer::GetObjectOfType(DXCopyBuffersTypeDef::GetTypeDef());
	rendering::DXCopyBuffers* copyBuffers = static_cast<rendering::DXCopyBuffers*>(obj);
	return copyBuffers;
}
