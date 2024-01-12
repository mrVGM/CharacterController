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
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList0;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList1;

		bool m_commnadListsRecorded[2] = {};

		Value m_swapChain;
		Value m_dsDescriptorHeap;
		Value m_commandQueue;

		void Create();

	protected:
		void LoadData(jobs::Job* done) override;

	public:

		ClearScreenRP(const ReferenceTypeDef& typeDef);
		virtual ~ClearScreenRP();

		void Prepare() override;
		void Execute() override;
	};
}