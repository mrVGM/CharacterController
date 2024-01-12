#pragma once

#include "CompositeTypeDef.h"
#include "CompositeValue.h"

#include "Job.h"

#include "MultiLoader.h"

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

	class DXCommandQueue : public ObjectValue, public jobs::LoadingClass
	{
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_graphicsCommandQueue;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_copyCommandQueue;
		void Create();

	protected:
		void LoadData(jobs::Job* done);

	public:
		DXCommandQueue(const ReferenceTypeDef& typeDef);
		virtual ~DXCommandQueue();

		ID3D12CommandQueue* GetGraphicsCommandQueue();
		ID3D12CommandQueue* GetCopyCommandQueue();
	};
}