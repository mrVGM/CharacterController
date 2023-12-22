#pragma once

#include "CompositeTypeDef.h"
#include "CompositeValue.h"

#include "Job.h"

#include <d3d12.h>
#include <wrl.h>
#include <string>

namespace rendering
{
	class DXCommandQueueTypeDef: public ReferenceTypeDef
	{
		TYPE_DEF_BODY(DXCommandQueueTypeDef)

	public:
		DXCommandQueueTypeDef();
		virtual ~DXCommandQueueTypeDef();

		void Construct(Value& value) const override;
	};

	class DXCommandQueue : public ObjectValue
	{
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;
		void Create();
	public:
		DXCommandQueue(const ReferenceTypeDef& typeDef);
		virtual ~DXCommandQueue();

		void Load(jobs::Job* done);

		ID3D12CommandQueue* GetCommandQueue();
	};
}