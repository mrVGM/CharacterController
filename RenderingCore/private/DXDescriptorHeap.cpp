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
}

const rendering::DXDescriptorHeapTypeDef& rendering::DXDescriptorHeapTypeDef::GetTypeDef()
{
	if (!m_heap.m_object)
	{
		m_heap.m_object = new DXDescriptorHeapTypeDef();
	}

	return *m_heap.m_object;
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
		jobs::RunSync(done);
	};

	auto loadTexJob = [=](DXTexture* tex) {
		return jobs::Job::CreateByLambda([=]() {
			tex->Load(jobs::Job::CreateByLambda(texLoaded));
		});
	};

	jobs::Job* init = jobs::Job::CreateByLambda([=]() {
		ValueList* defs = m_textureDefs.GetValue<ValueList*>();
		ValueList* tex = m_textures.GetValue<ValueList*>();

		for (auto it = defs->GetIterator(); it; ++it)
		{
			const TypeDef* curDef = (*it).GetType<const TypeDef*>();
			Value& cur = tex->EmplaceBack();
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

#undef THROW_ERROR