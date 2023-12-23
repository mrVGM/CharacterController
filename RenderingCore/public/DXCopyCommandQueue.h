#pragma once

#include "CompositeTypeDef.h"
#include "CompositeValue.h"

#include "Job.h"

#include <d3d12.h>
#include <wrl.h>

namespace rendering
{
	class DXCopyCommandQueueTypeDef: public ReferenceTypeDef
	{
		TYPE_DEF_BODY(DXCopyCommandQueueTypeDef)

	public:
		DXCopyCommandQueueTypeDef();
		virtual ~DXCopyCommandQueueTypeDef();

		void Construct(Value& container) const override;
	};

	class DXCopyCommandQueue : public ObjectValue
	{
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;
		void Create();
	public:
		DXCopyCommandQueue(const ReferenceTypeDef& typeDef);
		virtual ~DXCopyCommandQueue();

		void Load(jobs::Job* done);

		ID3D12CommandQueue* GetCommandQueue();
	};
}