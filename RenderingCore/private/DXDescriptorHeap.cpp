#include "DXDescriptorHeap.h"

#include "Jobs.h"

#include "ListDef.h"
#include "PrimitiveTypes.h"

#include "ValueList.h"
#include "ObjectValueContainer.h"

#include "CoreUtils.h"

#define THROW_ERROR(hRes, error) \
if (FAILED(hRes)) {\
    throw error;\
}

namespace
{
	BasicObjectContainer<rendering::DXDescriptorHeapTypeDef> m_heap;
	BasicObjectContainer<rendering::DepthStencilDescriptorHeapTypeDef> m_dsHeap;
	BasicObjectContainer<rendering::RenderTargetDescriptorHeapTypeDef> m_rtHeap;
	BasicObjectContainer<rendering::ShaderResourceDescriptorHeapTypeDef> m_srvHeap;
}

const rendering::DXDescriptorHeapTypeDef& rendering::DXDescriptorHeapTypeDef::GetTypeDef()
{
	if (!m_heap.m_object)
	{
		m_heap.m_object = new DXDescriptorHeapTypeDef();
	}

	return *m_heap.m_object;
}

const rendering::DepthStencilDescriptorHeapTypeDef& rendering::DepthStencilDescriptorHeapTypeDef::GetTypeDef()
{
	if (!m_dsHeap.m_object)
	{
		m_dsHeap.m_object = new DepthStencilDescriptorHeapTypeDef();
	}

	return *m_dsHeap.m_object;
}

const rendering::RenderTargetDescriptorHeapTypeDef& rendering::RenderTargetDescriptorHeapTypeDef::GetTypeDef()
{
	if (!m_rtHeap.m_object)
	{
		m_rtHeap.m_object = new RenderTargetDescriptorHeapTypeDef();
	}

	return *m_rtHeap.m_object;
}

const rendering::ShaderResourceDescriptorHeapTypeDef& rendering::ShaderResourceDescriptorHeapTypeDef::GetTypeDef()
{
	if (!m_srvHeap.m_object)
	{
		m_srvHeap.m_object = new ShaderResourceDescriptorHeapTypeDef();
	}

	return *m_srvHeap.m_object;
}

rendering::DXDescriptorHeapTypeDef::DXDescriptorHeapTypeDef() :
	ReferenceTypeDef(&ReferenceTypeDef::GetTypeDef(), "C997C11B-480D-4E43-A964-B213B30C4268"),
	m_textures("8BB3F252-3766-4EDD-8116-B168533B0C09", ListDef::GetTypeDef(TypeTypeDef::GetTypeDef(DXTextureTypeDef::GetTypeDef())))
{
	{
		m_textures.m_name = "Textures";
		m_textures.m_category = "Setup";
		m_textures.m_getValue = [](CompositeValue* obj) -> Value& {
			DXDescriptorHeap* heap = static_cast<DXDescriptorHeap*>(obj);
			return heap->m_textureDefs;
		};
		m_properties[m_textures.GetId()] = &m_textures;
	}

	m_name = "Descriptor Heap";
	m_category = "Rendering";
}

rendering::DXDescriptorHeapTypeDef::~DXDescriptorHeapTypeDef()
{
}

void rendering::DXDescriptorHeapTypeDef::Construct(Value& container) const
{
	throw "Can't construct Descriptor Heap from this class!";
}

void rendering::DXDescriptorHeap::LoadData(jobs::Job* done)
{
	struct Context
	{
		int m_loading = 0;
	};

	Context* ctx = new Context();

	auto texLoaded = [=]() {
		--ctx->m_loading;
		if (ctx->m_loading > 0)
		{
			return;
		}

		delete ctx;

		Init();
		jobs::RunSync(done);
	};

	auto loadTexJob = [=](DXTexture* tex) {
		return jobs::Job::CreateByLambda([=]() {
			tex->Load(jobs::Job::CreateByLambda(texLoaded));
		});
	};

	jobs::Job* init = jobs::Job::CreateByLambda([=]() {
		m_device.AssignObject(core::utils::GetDevice());

		ValueList* defs = m_textureDefs.GetValue<ValueList*>();
		ValueList* tex = m_textures.GetValue<ValueList*>();

		for (auto it = defs->GetIterator(); it; ++it)
		{
			const TypeDef* curDef = (*it).GetType<const TypeDef*>();
			Value& cur = tex->EmplaceBack();
			++m_size;

			ObjectValue* texObject = ObjectValueContainer::GetObjectOfType(*curDef);
			DXTexture* t = static_cast<DXTexture*>(texObject);
			cur.AssignObject(t);

			++ctx->m_loading;
			jobs::RunAsync(loadTexJob(t));
		}
	});
	
	jobs::RunSync(init);
}

void rendering::DXDescriptorHeap::Init()
{
}

rendering::DXDescriptorHeap::DXDescriptorHeap(const ReferenceTypeDef& typeDef) :
	ObjectValue(typeDef),
	m_loader(*this),
	m_textureDefs(DXDescriptorHeapTypeDef::GetTypeDef().m_textures.GetType(), this),
	m_device(DXDeviceTypeDef::GetTypeDef(), this),
	m_textures(ListDef::GetTypeDef(DXTextureTypeDef::GetTypeDef()), this)
{
}

rendering::DXDescriptorHeap::~DXDescriptorHeap()
{
}

ID3D12DescriptorHeap* rendering::DXDescriptorHeap::GetDescriptorHeap()
{
	return m_descriptorHeap.Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE rendering::DXDescriptorHeap::GetDescriptorHandle(UINT index)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE handle(m_descriptorHeap->GetCPUDescriptorHandleForHeapStart());

	for (UINT i = 0; i < index; ++i)
	{
		handle.Offset(m_descriptorSize);
	}

	return handle;
}

const Value& rendering::DXDescriptorHeap::GetTextures() const
{
	return m_textures;
}


rendering::DepthStencilDescriptorHeapTypeDef::DepthStencilDescriptorHeapTypeDef() :
	ReferenceTypeDef(&DXDescriptorHeapTypeDef::GetTypeDef(), "ABB10D3E-07D1-46A8-B697-60F454CE8029")
{
	m_name = "Depth Stencil Descriptor Heap";
	m_category = "Rendering";
}

rendering::DepthStencilDescriptorHeapTypeDef::~DepthStencilDescriptorHeapTypeDef()
{
}

void rendering::DepthStencilDescriptorHeapTypeDef::Construct(Value& container) const
{
	DepthStencilDescriptorHeap* heap = new DepthStencilDescriptorHeap(*this);
	container.AssignObject(heap);
}

void rendering::DepthStencilDescriptorHeap::Init()
{
	DXDevice* device = m_device.GetValue<DXDevice*>();
	ValueList* textures = m_textures.GetValue<ValueList*>();
	{
		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
		dsvHeapDesc.NumDescriptors = m_size;
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		THROW_ERROR(
			device->GetDevice().CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_descriptorHeap)),
			"Can't Create DSV Heap!"
		);

		m_descriptorSize = device->GetDevice().GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_descriptorHeap->GetCPUDescriptorHandleForHeapStart());
	for (auto it = textures->GetIterator(); it; ++it)
	{
		D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
		depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
		depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;
		device->GetDevice().CreateDepthStencilView((*it).GetValue<DXTexture*>()->GetTexture(), &depthStencilDesc, dsvHandle);

		dsvHandle.Offset(1, m_descriptorSize);
	}
}

rendering::DepthStencilDescriptorHeap::DepthStencilDescriptorHeap(const ReferenceTypeDef& typeDef) :
	DXDescriptorHeap(typeDef)
{
}

rendering::DepthStencilDescriptorHeap::~DepthStencilDescriptorHeap()
{
}

rendering::RenderTargetDescriptorHeapTypeDef::RenderTargetDescriptorHeapTypeDef() :
	ReferenceTypeDef(&DXDescriptorHeapTypeDef::GetTypeDef(), "45F79619-08F2-49A0-83D8-EC32471E6278")
{
	m_name = "Render Target Descriptor Heap";
	m_category = "Rendering";
}

rendering::RenderTargetDescriptorHeapTypeDef::~RenderTargetDescriptorHeapTypeDef()
{
}

void rendering::RenderTargetDescriptorHeapTypeDef::Construct(Value& container) const
{
	RenderTargetDescriptorHeap* heap = new RenderTargetDescriptorHeap(*this);
	container.AssignObject(heap);
}


void rendering::RenderTargetDescriptorHeap::Init()
{
	DXDevice* device = m_device.GetValue<DXDevice*>();
	ValueList* textures = m_textures.GetValue<ValueList*>();
	{
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = m_size;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		THROW_ERROR(
			device->GetDevice().CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_descriptorHeap)),
			"Can't create a descriptor heap!")

		m_descriptorSize = device->GetDevice().GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_descriptorHeap->GetCPUDescriptorHandleForHeapStart());

		for (auto it = textures->GetIterator(); it; ++it)
		{
			device->GetDevice().CreateRenderTargetView((*it).GetValue<DXTexture*>()->GetTexture(), nullptr, rtvHandle);
			rtvHandle.Offset(1, m_descriptorSize);
		}
	}
}

rendering::RenderTargetDescriptorHeap::RenderTargetDescriptorHeap(const ReferenceTypeDef& typeDef) :
	DXDescriptorHeap(typeDef)
{
}

rendering::RenderTargetDescriptorHeap::~RenderTargetDescriptorHeap()
{
}

rendering::ShaderResourceDescriptorHeapTypeDef::ShaderResourceDescriptorHeapTypeDef() :
	ReferenceTypeDef(&DXDescriptorHeapTypeDef::GetTypeDef(), "01A09818-C771-4548-B69F-F5098788FD5F")
{
	m_name = "Shader Resource Descriptor Heap";
	m_category = "Rendering";
}

rendering::ShaderResourceDescriptorHeapTypeDef::~ShaderResourceDescriptorHeapTypeDef()
{
}

void rendering::ShaderResourceDescriptorHeapTypeDef::Construct(Value& container) const
{
	ShaderResourceDescriptorHeap* heap = new ShaderResourceDescriptorHeap(*this);
	container.AssignObject(heap);
}


void rendering::ShaderResourceDescriptorHeap::Init()
{
	DXDevice* device = m_device.GetValue<DXDevice*>();
	ValueList* textures = m_textures.GetValue<ValueList*>();

	{
		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
		srvHeapDesc.NumDescriptors = m_size;
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		THROW_ERROR(
			device->GetDevice().CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_descriptorHeap)),
			"Can't create a descriptor heap!")

		m_descriptorSize = device->GetDevice().GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle(m_descriptorHeap->GetCPUDescriptorHandleForHeapStart());

		for (auto it = textures->GetIterator(); it; ++it)
		{
			DXTexture* tex = (*it).GetValue<DXTexture*>();
			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.Format = tex->GetTextureDescription().Format;
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture3D.MipLevels = 1;
			srvDesc.Texture2D.MipLevels = 1;

			device->GetDevice().CreateShaderResourceView(tex->GetTexture(), &srvDesc, srvHandle);
			srvHandle.Offset(1, m_descriptorSize);
		}
	}
}

rendering::ShaderResourceDescriptorHeap::ShaderResourceDescriptorHeap(const ReferenceTypeDef& typeDef) :
	DXDescriptorHeap(typeDef)
{
}

rendering::ShaderResourceDescriptorHeap::~ShaderResourceDescriptorHeap()
{
}

#undef THROW_ERROR