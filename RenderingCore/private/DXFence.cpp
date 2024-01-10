#include "DXFence.h"

#include "DXDevice.h"

#include "Jobs.h"

#include "ObjectValueContainer.h"

namespace
{
	BasicObjectContainer<rendering::DXFenceTypeDef> m_fence;
}

const rendering::DXFenceTypeDef& rendering::DXFenceTypeDef::GetTypeDef()
{
	if (!m_fence.m_object)
	{
		m_fence.m_object = new DXFenceTypeDef();
	}

	return *m_fence.m_object;
}

rendering::DXFenceTypeDef::DXFenceTypeDef() :
	ReferenceTypeDef(&ReferenceTypeDef::GetTypeDef(), "B5E6EB43-2F57-4CAE-97B1-AAB34F7A0100")
{
	m_name = "Fence";
	m_category = "Rendering";
}

rendering::DXFenceTypeDef::~DXFenceTypeDef()
{
}

void rendering::DXFenceTypeDef::Construct(Value& container) const
{
	DXFence* fence = new DXFence(*this);
	container.AssignObject(fence);
}

void rendering::DXFence::Create()
{
	DXDevice* device = static_cast<DXDevice*>(ObjectValueContainer::GetObjectOfType(DXDeviceTypeDef::GetTypeDef()));
	if (!device)
	{
		throw "No Device found!";
	}

	HRESULT hr = device->GetDevice().CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
	if (FAILED(hr))
	{
		throw "Can't Create Fence!";
	}
}

ID3D12Fence* rendering::DXFence::GetFence() const
{
	return m_fence.Get();
}

rendering::DXFence::DXFence(const ReferenceTypeDef& typeDef) :
	ObjectValue(typeDef)
{
}

rendering::DXFence::~DXFence()
{
}

void rendering::DXFence::Load(jobs::Job* done)
{
	struct Context
	{
		DXFence* m_self = nullptr;
		jobs::Job* m_done = nullptr;
	};

	Context ctx{ this, done };

	class LoadJob : public jobs::Job
	{
	private:
		Context m_ctx;
	public:
		LoadJob(const Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			m_ctx.m_self->Create();

			jobs::RunSync(m_ctx.m_done);
		}
	};


	jobs::RunSync(new LoadJob(ctx));
}

