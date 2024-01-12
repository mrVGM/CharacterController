#include "DXMutableBuffer.h"

#include "Jobs.h"

#include "DXHeap.h"
#include "DXBuffer.h"
#include "DXDevice.h"

#include "ObjectValueContainer.h"

#define THROW_ERROR(hRes, error) \
if (FAILED(hRes)) {\
    throw error;\
}

namespace
{
	BasicObjectContainer<rendering::DXMutableBufferTypeDef> m_mutableBuffer;
	int m_fenceProgress = 1;
}

const rendering::DXMutableBufferTypeDef& rendering::DXMutableBufferTypeDef::GetTypeDef()
{
	if (!m_mutableBuffer.m_object)
	{
		m_mutableBuffer.m_object = new DXMutableBufferTypeDef();
	}

	return *m_mutableBuffer.m_object;
}

rendering::DXMutableBufferTypeDef::DXMutableBufferTypeDef() :
	ReferenceTypeDef(&ReferenceTypeDef::GetTypeDef(), "4EA9B059-93A0-4110-95DC-772ABAF531E4")
{
	m_name = "Mutable Buffer";
	m_category = "Rendering";
}

rendering::DXMutableBufferTypeDef::~DXMutableBufferTypeDef()
{
}

void rendering::DXMutableBufferTypeDef::Construct(Value& container) const
{
	DXMutableBuffer* buffer = new DXMutableBuffer(*this);
	container.AssignObject(buffer);
}



rendering::DXMutableBuffer::DXMutableBuffer(const ReferenceTypeDef& typeDef) :
	ObjectValue(typeDef),
	m_buffer(DXBufferTypeDef::GetTypeDef(), this),
	m_uploadBuffer(DXBufferTypeDef::GetTypeDef(), this)
{
	DXBufferTypeDef::GetTypeDef().Construct(m_buffer);
	DXBufferTypeDef::GetTypeDef().Construct(m_uploadBuffer);
}

rendering::DXMutableBuffer::~DXMutableBuffer()
{
}

void rendering::DXMutableBuffer::SetSizeAndStride(UINT64 size, UINT64 stride)
{
	m_size = size;
	m_stride = stride;

	DXBuffer* buffer = m_buffer.GetValue<DXBuffer*>();
	DXBuffer* uploadBuffer = m_uploadBuffer.GetValue<DXBuffer*>();

	buffer->SetBufferSizeAndFlags(m_size, D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE);
	buffer->SetBufferStride(m_stride);

	uploadBuffer->SetBufferSizeAndFlags(m_size, D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE);
	uploadBuffer->SetBufferStride(m_stride);
}

void rendering::DXMutableBuffer::Load(jobs::Job* done)
{
	struct Context
	{
		int m_toLoad = 2;
		Value m_heapVal;
		Value m_uploadHeapVal;

		Context() :
			m_heapVal(DXHeapTypeDef::GetTypeDef(), nullptr),
			m_uploadHeapVal(DXHeapTypeDef::GetTypeDef(), nullptr)
		{
		}
	};
	Context* ctx = new Context();

	auto getHeap = [=]() {
		return ctx->m_heapVal.GetValue<DXHeap*>();
	};

	auto getUploadHeap = [=]() {
		return ctx->m_uploadHeapVal.GetValue<DXHeap*>();
	};

	auto heapLoaded = [=]() {
		--ctx->m_toLoad;
		if (ctx->m_toLoad > 0)
		{
			return;
		}

		DXBuffer* buffer = m_buffer.GetValue<DXBuffer*>();
		DXBuffer* uploadBuffer = m_uploadBuffer.GetValue<DXBuffer*>();

		DXHeap* heap = getHeap();
		DXHeap* uploadHeap = getUploadHeap();

		buffer->Place(heap, 0);
		uploadBuffer->Place(uploadHeap, 0);

		delete ctx;

		CreateCommandList();

		m_isLoaded = true;
		jobs::RunSync(done);
	};


	jobs::Job* init = jobs::Job::CreateByLambda([=]() {
		DXHeapTypeDef::GetTypeDef().Construct(ctx->m_heapVal);
		DXHeapTypeDef::GetTypeDef().Construct(ctx->m_uploadHeapVal);

		DXHeap* heap = getHeap();
		DXHeap* uploadHeap = getUploadHeap();

		heap->SetHeapSize(m_size);
		heap->SetHeapType(D3D12_HEAP_TYPE_DEFAULT);
		heap->SetHeapFlags(D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS);

		uploadHeap->SetHeapSize(m_size);
		uploadHeap->SetHeapType(D3D12_HEAP_TYPE_UPLOAD);
		uploadHeap->SetHeapFlags(D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS);

		jobs::RunAsync(jobs::Job::CreateByLambda([=]() {
			heap->MakeResident(jobs::Job::CreateByLambda(heapLoaded));
		}));

		jobs::RunAsync(jobs::Job::CreateByLambda([=]() {
			uploadHeap->MakeResident(jobs::Job::CreateByLambda(heapLoaded));
		}));
	});

	jobs::RunSync(init);
}

void rendering::DXMutableBuffer::Upload(jobs::Job* done)
{
	m_uploadBuffer.GetValue<DXBuffer*>()->CopyBuffer(*m_buffer.GetValue<DXBuffer*>(), done);
}

ID3D12CommandList* rendering::DXMutableBuffer::GetCopyCommandList()
{
	return m_commandList.Get();
}

void rendering::DXMutableBuffer::SetDirty(bool dirty)
{
	if (m_isLoaded)
	{
		m_isDirty = dirty;
	}
}

bool rendering::DXMutableBuffer::IsDirty()
{
	return m_isDirty;
}

void rendering::DXMutableBuffer::CreateCommandList()
{
	Value deviceVal(DXDeviceTypeDef::GetTypeDef(), nullptr);
	ObjectValueContainer::GetObjectOfType(DXDeviceTypeDef::GetTypeDef(), deviceVal);
	DXDevice* device = deviceVal.GetValue<DXDevice*>();

	THROW_ERROR(
		device->GetDevice().CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY, IID_PPV_ARGS(&m_commandAllocator)),
		"Can't create Command Allocator!")

	THROW_ERROR(
		device->GetDevice().CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COPY, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList)),
		"Can't create Command List!")

	m_commandList->CopyResource(m_buffer.GetValue<DXBuffer*>()->GetBuffer(), m_uploadBuffer.GetValue<DXBuffer*>()->GetBuffer());

	THROW_ERROR(
		m_commandList->Close(),
		"Can't close command List!")
}

#undef THROW_ERROR