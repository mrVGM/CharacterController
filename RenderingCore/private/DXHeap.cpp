#include "DXHeap.h"

#include "JobSystem.h"
#include "Jobs.h"

#include "WaitFence.h"

#include "CoreUtils.h"

namespace
{
	BasicObjectContainer<rendering::DXHeapTypeDef> m_heap;
	int m_fenceProgress = 1;
}

const rendering::DXHeapTypeDef& rendering::DXHeapTypeDef::GetTypeDef()
{
	if (!m_heap.m_object)
	{
		m_heap.m_object = new DXHeapTypeDef();
	}

	return *m_heap.m_object;
}

rendering::DXHeapTypeDef::DXHeapTypeDef() :
	ReferenceTypeDef(&ReferenceTypeDef::GetTypeDef(), "7475F45E-E538-4F8E-811B-9D16A7861D7E")
{
	m_name = "Heap";
	m_category = "Rendering";
}

rendering::DXHeapTypeDef::~DXHeapTypeDef()
{
}

void rendering::DXHeapTypeDef::Construct(Value& container) const
{
	DXHeap* heap = new DXHeap(*this);
	container.AssignObject(heap);
}

rendering::DXHeap::DXHeap(const ReferenceTypeDef& typeDef) :
	ObjectValue(typeDef)
{
	m_heapDescription.SizeInBytes = 256;
	m_heapDescription.Properties.Type = D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_DEFAULT;
	m_heapDescription.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	m_heapDescription.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	m_heapDescription.Properties.CreationNodeMask = 0;
	m_heapDescription.Properties.VisibleNodeMask = 0;

	m_heapDescription.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	m_heapDescription.Flags =
		D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_CREATE_NOT_ZEROED |
		D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_CREATE_NOT_RESIDENT;
}

rendering::DXHeap::~DXHeap()
{
	if (m_resident) {
		Evict();
	}
}

void rendering::DXHeap::MakeResident(jobs::Job* done)
{
	if (m_resident) {
		throw "The heap is already Resident!";
	}

	struct Context
	{
		jobs::JobSystem* m_residentHeapJS = nullptr;
		DXFence* m_residentHeapFence = nullptr;
		DXDevice* m_device = nullptr;
		int m_signal = 0;

		DXHeap* m_self = nullptr;
		jobs::Job* m_done = nullptr;
	};

	Context ctx;
	ctx.m_self = this;
	ctx.m_done = done;


	class WaitJob : public jobs::Job
	{
	private:
		Context m_ctx;
	public:
		WaitJob(const Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			WaitFence waitFence(*m_ctx.m_residentHeapFence);
			waitFence.Wait(m_ctx.m_signal);
			m_ctx.m_self->m_resident = true;

			jobs::RunSync(m_ctx.m_done);
		}
	};

	class EnqueJob : public jobs::Job
	{
	private:
		Context m_ctx;
	public:
		EnqueJob(const Context& jobContext) :
			m_ctx(jobContext)
		{
		}

		void Do() override
		{
			ID3D12Device3* device3;
			HRESULT hr = m_ctx.m_device->GetDevice().QueryInterface(IID_PPV_ARGS(&device3));
			if (FAILED(hr))
			{
				throw "Can't Query ID3D12Device3!";
			}
			const UINT64 signal = m_fenceProgress++;
			ID3D12Pageable* tmp = m_ctx.m_self->GetHeap();
			hr = device3->EnqueueMakeResident(D3D12_RESIDENCY_FLAGS::D3D12_RESIDENCY_FLAG_DENY_OVERBUDGET, 1, &tmp, m_ctx.m_residentHeapFence->GetFence(), signal);
			if (FAILED(hr))
			{
				throw "Can't make the heap resident!";
			}

			m_ctx.m_signal = signal;
			WaitJob* waitJob = new WaitJob(m_ctx);
			jobs::RunAsync(waitJob);
		}
	};
	
	class CacheObjects : public jobs::Job
	{
	private:
		Context m_ctx;
	public:
		CacheObjects(const Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			m_ctx.m_device = core::utils::GetDevice();
			m_ctx.m_residentHeapJS = core::utils::GetResidentHeapJobSystem();
			m_ctx.m_residentHeapFence = core::utils::GetResidentHeapFence();

			m_ctx.m_self->Create();
			m_ctx.m_residentHeapJS->ScheduleJob(new EnqueJob(m_ctx));
		}
	};

	jobs::RunSync(new CacheObjects(ctx));
}

void rendering::DXHeap::Evict()
{
	if (!m_resident) {
		throw "The heap is not Resident yet!";
	}

	DXDevice* device = core::utils::GetDevice();
	ID3D12Device3* device3;
	HRESULT hr = device->GetDevice().QueryInterface(IID_PPV_ARGS(&device3));
	if (FAILED(hr)) {
		throw "Can't Query ID3D12Device3!";
	}

	ID3D12Pageable* const tmp = m_heap.Get();
	hr = device3->Evict(1, &tmp);
	if (FAILED(hr))
	{
		throw "Can't Evict the Heap!";
	}

	m_resident = false;
}

ID3D12Heap* rendering::DXHeap::GetHeap() const
{
	return m_heap.Get();
}

void rendering::DXHeap::SetHeapSize(UINT64 size)
{
	m_heapDescription.SizeInBytes = size;
}

void rendering::DXHeap::SetHeapType(D3D12_HEAP_TYPE type)
{
	m_heapDescription.Properties.Type = type;
}

void rendering::DXHeap::SetHeapFlags(D3D12_HEAP_FLAGS flags)
{
	m_heapDescription.Flags = m_heapDescription.Flags | flags;
}

void rendering::DXHeap::Create()
{
	DXDevice* device = core::utils::GetDevice();
	HRESULT hr = device->GetDevice().CreateHeap(&m_heapDescription, IID_PPV_ARGS(&m_heap));
	if (FAILED(hr))
	{
		throw "Can't create Heap!";
	}
}

const D3D12_HEAP_DESC& rendering::DXHeap::GetDescription() const
{
	return m_heapDescription;
}