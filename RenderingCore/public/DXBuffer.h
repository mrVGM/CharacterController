#pragma once

#include "CompositeTypeDef.h"
#include "CompositeValue.h"

#include "Job.h"
#include "DXHeap.h"

#include "d3dx12.h"
#include <wrl.h>

namespace rendering
{
    class DXBufferTypeDef : public ReferenceTypeDef
    {
        TYPE_DEF_BODY(DXBufferTypeDef)

    public:
		DXBufferTypeDef();
        virtual ~DXBufferTypeDef();

        void Construct(Value& container) const override;
    };

	class DXBuffer : public ObjectValue
	{
		Value m_heap;
		CD3DX12_RESOURCE_DESC m_bufferDescription = {};
		Microsoft::WRL::ComPtr<ID3D12Resource> m_buffer;

		UINT64 m_size = -1;
		UINT64 m_stride = -1;

	public:
		ID3D12Resource* GetBuffer() const;
		UINT64 GetBufferSize() const;
		UINT64 GetStride() const;
		UINT64 GetElementCount() const;

		DXBuffer(const ReferenceTypeDef& typeDef);
		virtual ~DXBuffer();

		void CopyData(void* data, int dataSize);
		void CopyBuffer(
			rendering::DXBuffer& destination,
			jobs::Job* done) const;

		void Place(DXHeap* heap, UINT64 heapOffset);

		void SetBufferSizeAndFlags(UINT64 size, D3D12_RESOURCE_FLAGS flags);
		void SetBufferStride(UINT64 size);

		void* Map();
		void Unmap();

		DXHeap* GetResidentHeap() const;
	};
}