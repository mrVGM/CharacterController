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
		TypeProperty m_quadMesh;

		UnlitRPTypeDef();
		virtual ~UnlitRPTypeDef();

		void Construct(Value& container) const override;
	};

	class UnlitRP : public render_pass::RenderPass
	{
	private:
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_auxCommandAllocator;

		struct CMDListCache
		{
			bool m_cached = false;
			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_beginCommandList;
			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_afterRenderObjects;
			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_endCommandList;
		};

		CMDListCache m_cache[2];

		std::list<Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>> m_commandLists;

		Value m_device;
		Value m_swapChain;
		Value m_commandQueue;

		Value m_scene;

		Value m_unlitMaterial;
		Value m_displayTextureMat;

		Value m_quadMesh;

		void Create();
		const CMDListCache& GetCachedLists();
	public:
		Value m_quadMeshDef;
		Value m_displayTextureMatDef;

		UnlitRP(const ReferenceTypeDef& typeDef);
		virtual ~UnlitRP();

		void Prepare() override;
		void Execute() override;
		void Load(jobs::Job* done) override;
	};

	void Boot();
}