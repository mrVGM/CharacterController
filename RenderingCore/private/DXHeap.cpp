#include "DXHeap.h"

#include "JobSystem.h"
#include "Jobs.h"

#include "WaitFence.h"

#include "DXDevice.h"
#include "ResidentHeapFence.h"
#include "ResidentHeapJobSystem.h"

#include "DXFence.h"

#include "ObjectValueContainer.h"

namespace
{
	BasicObjectContainer<rendering::DXHeapTypeDef> m_heap;
	UINT64 m_fenceProgress = 1;
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
	ObjectValue(typeDef),
	m_device(DXDeviceTypeDef::GetTypeDef(), this),
	m_residentHeapJS(jobs::JobSystemDef::GetTypeDef(), this),
	m_residentHeapFence(DXFenceTypeDef::GetTypeDef(), this)
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

void rendering::DXHeap::MakeResident(jobs::Job done)
{
	if (m_resident) {
		throw "The heap is already Resident!";
	}

	auto createWaitJob = [=](UINT64 signal) {
		return [=]() {
			DXFence* fence = m_residentHeapFence.GetValue<DXFence*>();
			WaitFence waitFence(*fence);
			waitFence.Wait(signal);
			m_resident = true;

			jobs::RunSync(done);
		};
	};

	jobs::Job enqueJob = [=]() {
		ID3D12Device3* device3;
		HRESULT hr = m_device.GetValue<DXDevice*>()->GetDevice().QueryInterface(IID_PPV_ARGS(&device3));
		if (FAILED(hr))
		{
			throw "Can't Query ID3D12Device3!";
		}
		const UINT64 signal = m_fenceProgress++;
		ID3D12Pageable* tmp = GetHeap();
		hr = device3->EnqueueMakeResident(D3D12_RESIDENCY_FLAGS::D3D12_RESIDENCY_FLAG_DENY_OVERBUDGET, 1, &tmp, m_residentHeapFence.GetValue<DXFence*>()->GetFence(), signal);
		if (FAILED(hr))
		{
			throw "Can't make the heap resident!";
		}

		jobs::RunAsync(createWaitJob(signal));
	};

	jobs::Job init = [=]() {
		ObjectValueContainer::GetObjectOfType(DXDeviceTypeDef::GetTypeDef(), m_device);
		ObjectValueContainer::GetObjectOfType(ResidentHeapJobSystemTypeDef::GetTypeDef(), m_residentHeapJS);
		ObjectValueContainer::GetObjectOfType(ResidentHeapFenceTypeDef::GetTypeDef(), m_residentHeapFence);

		Create();
		m_residentHeapJS.GetValue<jobs::JobSystem*>()->ScheduleJob(enqueJob);
	};

	jobs::RunSync(init);
}

void rendering::DXHeap::Evict()
{
	if (!m_resident) {
		throw "The heap is not Resident yet!";
	}
	DXDevice* device = m_device.GetValue<DXDevice*>();

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
	DXDevice* device = m_device.GetValue<DXDevice*>();

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