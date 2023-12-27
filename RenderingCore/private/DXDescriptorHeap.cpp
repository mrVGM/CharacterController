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

void rendering::DXDescriptorHeap::Load(jobs::Job* done)
{
	m_loader.Load(done);
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

#undef THROW_ERROR