#pragma once

#include "RenderPass.h"

#include "d3dx12.h"
#include <wrl.h>

namespace rendering::render_pass
{
	class ClearScreenRPTypeDef : public ReferenceTypeDef
	{
		TYPE_DEF_BODY(ClearScreenRPTypeDef)

	public:
		ClearScreenRPTypeDef();
		virtual ~ClearScreenRPTypeDef();

		void Construct(Value& container) const override;
	};

	class ClearScreenRP : public RenderPass
	{
	private:
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;

		Value m_swapChain;
		Value m_commandQueue;

		void Create();
	public:

		ClearScreenRP(const ReferenceTypeDef& typeDef);
		virtual ~ClearScreenRP();

		void Prepare() override;
		void Execute() override;
		void Load(jobs::Job* done) override;
	};
}