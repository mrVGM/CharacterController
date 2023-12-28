#pragma once

#include "CompositeTypeDef.h"
#include "CompositeValue.h"

#include "RenderPass.h"

#include "d3dx12.h"
#include <wrl.h>

namespace rendering::unlit_rp
{
	class UnlitRPTypeDef : public ReferenceTypeDef
	{
		TYPE_DEF_BODY(UnlitRPTypeDef)

	public:
		TypeProperty m_displayTextureMat;

		UnlitRPTypeDef();
		virtual ~UnlitRPTypeDef();

		void Construct(Value& container) const override;
	};

	class UnlitRP : public render_pass::RenderPass
	{
	private:
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_beginCommandList;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_endCommandList;

		std::list<Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>> m_commandLists;

		Value m_device;
		Value m_swapChain;
		Value m_commandQueue;

		Value m_scene;

		Value m_unlitMaterial;
		Value m_displayTextureMat;

		void Create();
	public:
		Value m_displayTextureMatDef;

		UnlitRP(const ReferenceTypeDef& typeDef);
		virtual ~UnlitRP();

		void Prepare() override;
		void Execute() override;
		void Load(jobs::Job* done) override;
	};

	void Boot();
}