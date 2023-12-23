#pragma once

#include "CompositeTypeDef.h"
#include "CompositeValue.h"

#include "Job.h"

#include "d3dx12.h"
#include <wrl.h>

namespace rendering
{
    class DXHeapTypeDef : public ReferenceTypeDef
    {
        TYPE_DEF_BODY(DXHeapTypeDef)

    public:
		DXHeapTypeDef();
        virtual ~DXHeapTypeDef();

        void Construct(Value& container) const override;
    };

	class DXHeap : public ObjectValue
	{
		D3D12_HEAP_DESC m_heapDescription = {};
		bool m_resident = false;

		Microsoft::WRL::ComPtr<ID3D12Heap> m_heap;
		void Evict();
		void Create();

	public:
		DXHeap(const ReferenceTypeDef& typeDef);
		virtual ~DXHeap();

		void MakeResident(jobs::Job* done);
		ID3D12Heap* GetHeap() const;
		void SetHeapSize(UINT64 size);
		void SetHeapType(D3D12_HEAP_TYPE type);
		void SetHeapFlags(D3D12_HEAP_FLAGS flags);

		const D3D12_HEAP_DESC& GetDescription() const;
	};
}