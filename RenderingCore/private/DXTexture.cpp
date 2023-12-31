#include "DXTexture.h"

#include "DXHeap.h"

#include "Jobs.h"

#include "CoreUtils.h"

#define THROW_ERROR(hRes, error) \
if (FAILED(hRes)) {\
    throw error;\
}

namespace
{
	BasicObjectContainer<rendering::DXTextureTypeDef> m_texture;
}

const rendering::DXTextureTypeDef& rendering::DXTextureTypeDef::GetTypeDef()
{
	if (!m_texture.m_object)
	{
		m_texture.m_object = new DXTextureTypeDef();
	}

	return *m_texture.m_object;
}

rendering::DXTextureTypeDef::DXTextureTypeDef() :
	ReferenceTypeDef(&ReferenceTypeDef::GetTypeDef(), "A1693B2C-8BE1-4AF8-AAC5-00206DA6FDE6")
{
	m_name = "Texture";
	m_category = "Rendering";
}

rendering::DXTextureTypeDef::~DXTextureTypeDef()
{
}

void rendering::DXTextureTypeDef::Construct(Value& container) const
{
	throw "Can't create a texture from this type!";
}


rendering::DXTexture::DXTexture(const ReferenceTypeDef& typeDef) :
	ObjectValue(typeDef),
	m_device(DXDeviceTypeDef::GetTypeDef(), this),
	m_heap(DXHeapTypeDef::GetTypeDef(), this)
{
}

rendering::DXTexture::~DXTexture()
{
}

const D3D12_RESOURCE_DESC& rendering::DXTexture::GetTextureDescription() const
{
	return m_description;
}

void rendering::DXTexture::SetDescription(const D3D12_RESOURCE_DESC& description)
{
	m_description = description;
}

D3D12_RESOURCE_ALLOCATION_INFO rendering::DXTexture::GetTextureAllocationInfo() const
{
	D3D12_RESOURCE_DESC textureDesc = GetTextureDescription();
	D3D12_RESOURCE_ALLOCATION_INFO info = m_device.GetValue<DXDevice*>()->GetDevice().GetResourceAllocationInfo(0, 1, &textureDesc);
	return info;
}

ID3D12Resource* rendering::DXTexture::GetTexture() const
{
	return m_texture.Get();
}

void rendering::DXTexture::LoadData(jobs::Job* done)
{
	auto getHeap = [=]() {
		return m_heap.GetValue<DXHeap*>();
	};

	jobs::Job* placeTex = jobs::Job::CreateByLambda([=]() {
		DXHeap* heap = getHeap();
		Place(*heap, 0);

		jobs::RunSync(done);
	});

	jobs::Job* loadHeap = jobs::Job::CreateByLambda([=]() {
		DXHeap* heap = getHeap();
		heap->SetHeapType(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_DEFAULT);
		heap->SetHeapFlags(D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES);
		heap->SetHeapSize(GetTextureAllocationInfo().SizeInBytes);

		heap->MakeResident(placeTex);
	});

	jobs::Job* init = jobs::Job::CreateByLambda([=]() {
		m_device.AssignObject(core::utils::GetDevice());
		DXHeapTypeDef::GetTypeDef().Construct(m_heap);

		jobs::RunAsync(loadHeap);
	});

	jobs::RunSync(init);
}

void rendering::DXTexture::Place(DXHeap& heap, UINT64 heapOffset)
{
	DXDevice* device = m_device.GetValue<DXDevice*>();

	D3D12_RESOURCE_DESC textureDesc = GetTextureDescription();
	bool isDS = textureDesc.Flags & D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
	depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
	depthOptimizedClearValue.DepthStencil.Stencil = 0;

	D3D12_CLEAR_VALUE regularClearValue = {};
	regularClearValue.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	THROW_ERROR(
		device->GetDevice().CreatePlacedResource(
			heap.GetHeap(),
			heapOffset,
			&textureDesc,
			isDS ? D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_DEPTH_WRITE : D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT,
			isDS ? &depthOptimizedClearValue : &regularClearValue,
			IID_PPV_ARGS(&m_texture)),
		"Can't place texture in the heap!")
}

D3D12_RESOURCE_DESC rendering::DXTexture::CreateDepthStencilTextureDescription(UINT width, UINT height)
{
	D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	DXGI_FORMAT format = DXGI_FORMAT_D32_FLOAT;

	CD3DX12_RESOURCE_DESC textureDesc = {};
	textureDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		format,
		width,
		height,
		1,
		0,
		1,
		0,
		flags);

	return textureDesc;
}


D3D12_RESOURCE_DESC rendering::DXTexture::CreateRenderTargetTextureDescription(UINT width, UINT height)
{
	D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	DXGI_FORMAT format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	CD3DX12_RESOURCE_DESC textureDesc = {};
	textureDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		format,
		width,
		height,
		1,
		0,
		1,
		0,
		flags);

	return textureDesc;
}

#undef THROW_ERROR