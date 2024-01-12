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
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_auxCommandAllocator;

		int m_commandListsSize = 0;
		ID3D12CommandList** m_commandLists = nullptr;

		struct CMDListCache
		{
			bool m_cached = false;
			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_beginCommandList;
			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_afterRenderObjects;
			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_endCommandList;
			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_displayRTCommandList;
		};

		CMDListCache m_cache[2];

		Value m_device;
		Value m_swapChain;
		Value m_commandQueue;

		Value m_scene;

		Value m_unlitMaterial;
		Value m_displayTextureMat;

		Value m_quadMesh;

		void Create();
		const CMDListCache& GetCachedLists();
		void ResizeCommandLists(int size);

	protected:
		void LoadData(jobs::Job* done) override;

	public:
		Value m_quadMeshDef;
		Value m_displayTextureMatDef;

		UnlitRP(const ReferenceTypeDef& typeDef);
		virtual ~UnlitRP();

		void Prepare() override;
		void Execute() override;
	};

	void Boot();
}