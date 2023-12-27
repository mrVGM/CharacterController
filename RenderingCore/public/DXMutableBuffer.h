#pragma once

#include "CompositeTypeDef.h"
#include "CompositeValue.h"

#include "Job.h"

#include "d3dx12.h"
#include <wrl.h>

namespace rendering
{
    class DXMutableBufferTypeDef : public ReferenceTypeDef
    {
        TYPE_DEF_BODY(DXMutableBufferTypeDef)

    public:
		DXMutableBufferTypeDef();
        virtual ~DXMutableBufferTypeDef();

        void Construct(Value& container) const override;
    };

	class DXMutableBuffer : public ObjectValue
	{
	private:
		bool m_isLoaded = false;
		bool m_isDirty = false;

		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;

		UINT64 m_size;
		UINT64 m_stride;

		void CreateCommandList();
	public:
		Value m_buffer;
		Value m_uploadBuffer;

		DXMutableBuffer(const ReferenceTypeDef& typeDef);
		virtual ~DXMutableBuffer();

		void SetSizeAndStride(UINT64 size, UINT64 stride);

		void SetDirty(bool dirty);
		bool IsDirty();

		void Load(jobs::Job* done);
		void Upload(jobs::Job* done);

		ID3D12CommandList* GetCopyCommandList();
	};
}