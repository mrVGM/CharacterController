#include "CoreUtils.h"

#include "RenderFence.h"
#include "ResidentHeapFence.h"
#include "ResidentHeapJobSystem.h"

#include "DXDepthStencilTexture.h"

#include "ObjectValueContainer.h"

namespace
{
	D3D12_INPUT_ELEMENT_DESC m_3DMaterialInputlayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	D3D12_INPUT_ELEMENT_DESC m_3DSkeletalMeshMaterialInputlayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },

		{ "OBJECT_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 0 },
		{ "OBJECT_ROTATION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 12, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 0 },
		{ "OBJECT_SCALE", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 28, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 0 },

		{ "JOINT_1_INDEX", 0, DXGI_FORMAT_R32G32B32A32_SINT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "JOINT_2_INDEX", 0, DXGI_FORMAT_R32G32B32A32_SINT, 2, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "JOINT_1_WEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 2, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "JOINT_2_WEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 2, 48, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};
}

void rendering::core::utils::Get3DMaterialInputLayout(const D3D12_INPUT_ELEMENT_DESC*& layout, unsigned int& numElements)
{
	layout = m_3DMaterialInputlayout;
	numElements = _countof(m_3DMaterialInputlayout);
}

void rendering::core::utils::Get3DSkeletalMeshMaterialInputLayout(const D3D12_INPUT_ELEMENT_DESC*& layout, unsigned int& numElements)
{
	layout = m_3DSkeletalMeshMaterialInputlayout;
	numElements = _countof(m_3DSkeletalMeshMaterialInputlayout);
}



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

rendering::DXTexture* rendering::core::utils::GetDepthStencilTexture()
{
	ObjectValue* obj = ObjectValueContainer::GetObjectOfType(DXDepthStencilTextureTypeDef::GetTypeDef());
	rendering::DXTexture* tex = static_cast<rendering::DXTexture*>(obj);
	return tex;
}