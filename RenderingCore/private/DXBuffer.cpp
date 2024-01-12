#include "DXBuffer.h"

#include "DXHeap.h"

#include "Jobs.h"

#include "DXDevice.h"
#include "DXCopyBuffers.h"

#include "ObjectValueContainer.h"

namespace
{
	BasicObjectContainer<rendering::DXBufferTypeDef> m_buffer;
}

const rendering::DXBufferTypeDef& rendering::DXBufferTypeDef::GetTypeDef()
{
	if (!m_buffer.m_object)
	{
		m_buffer.m_object = new DXBufferTypeDef();
	}

	return *m_buffer.m_object;
}

rendering::DXBufferTypeDef::DXBufferTypeDef() :
	ReferenceTypeDef(&ReferenceTypeDef::GetTypeDef(), "5C21105E-EC30-4A1A-A31B-63A36215FF4D")
{
	m_name = "Buffer";
	m_category = "Rendering";
}

rendering::DXBufferTypeDef::~DXBufferTypeDef()
{
}

void rendering::DXBufferTypeDef::Construct(Value& container) const
{
	DXBuffer* buffer = new DXBuffer(*this);
	container.AssignObject(buffer);
}


void rendering::DXBuffer::SetBufferSizeAndFlags(UINT64 size, D3D12_RESOURCE_FLAGS flags)
{
	m_size = size;
	m_bufferDescription = CD3DX12_RESOURCE_DESC::Buffer(m_size, flags);
}

void rendering::DXBuffer::SetBufferStride(UINT64 stride)
{
	m_stride = stride;
}

rendering::DXBuffer::DXBuffer(const ReferenceTypeDef& typeDef) :
	ObjectValue(typeDef),
	m_device(DXDeviceTypeDef::GetTypeDef(), this),
	m_copyBuffers(DXCopyBuffersTypeDef::GetTypeDef(), this),
	m_heap(DXHeapTypeDef::GetTypeDef(), this)
{
}

rendering::DXBuffer::~DXBuffer()
{
}

void rendering::DXBuffer::CopyData(void* data, int dataSize)
{
	CD3DX12_RANGE readRange(0, 0);

	void* dst = nullptr;

	HRESULT hRes = m_buffer->Map(0, &readRange, &dst);
	if (FAILED(hRes))
	{
		throw "Can't map Vertex Buffer!";
	}

	memcpy(dst, data, dataSize);
	m_buffer->Unmap(0, nullptr);
}


void rendering::DXBuffer::Place(DXHeap* heap, UINT64 heapOffset)
{
	ObjectValueContainer::GetObjectOfType(DXDeviceTypeDef::GetTypeDef(), m_device);
	DXDevice* device = m_device.GetValue<DXDevice*>();
	D3D12_HEAP_TYPE heapType = heap->GetDescription().Properties.Type;

	D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT;
	if (heapType == D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD)
	{
		initialState = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ;
	}
	if (heapType == D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_READBACK)
	{
		initialState = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST;
	}

	HRESULT hRes = device->GetDevice().CreatePlacedResource(heap->GetHeap(), heapOffset, &m_bufferDescription, initialState, nullptr, IID_PPV_ARGS(&m_buffer));
	if (FAILED(hRes))
	{
		throw "Can't place buffer in the heap!";
	}

	m_heap.AssignObject(heap);
}

ID3D12Resource* rendering::DXBuffer::GetBuffer() const
{
	return m_buffer.Get();
}

UINT64 rendering::DXBuffer::GetBufferSize() const
{
	return m_size;
}

UINT64 rendering::DXBuffer::GetStride() const
{
	return m_stride;
}

UINT64 rendering::DXBuffer::GetElementCount() const
{
	return m_size / m_stride;
}


void rendering::DXBuffer::CopyBuffer(
	rendering::DXBuffer& destination,
	jobs::Job* done) const
{
	DXBuffer* destBuff = &destination;
	
	jobs::Job* init = jobs::Job::CreateByLambda([=]() {
		DXBuffer* self = const_cast<DXBuffer*>(this);
		DXCopyBuffers* copyBuffers = m_copyBuffers.GetValue<DXCopyBuffers*>();
		if (!copyBuffers)
		{
			ObjectValueContainer::GetObjectOfType(DXCopyBuffersTypeDef::GetTypeDef(), self->m_copyBuffers);
			copyBuffers = m_copyBuffers.GetValue<DXCopyBuffers*>();
		}

		copyBuffers->Execute(*destBuff, *this, done);
	});

	jobs::RunSync(init);
}

void* rendering::DXBuffer::Map()
{
	CD3DX12_RANGE readRange(0, 0);

	void* dst = nullptr;

	HRESULT hRes = m_buffer->Map(0, &readRange, &dst);
	if (FAILED(hRes))
	{
		throw "Can't map Vertex Buffer!";
	}

	return dst;
}

void rendering::DXBuffer::Unmap()
{
	m_buffer->Unmap(0, nullptr);
}

rendering::DXHeap* rendering::DXBuffer::GetResidentHeap() const
{
	return m_heap.GetValue<DXHeap*>();
}