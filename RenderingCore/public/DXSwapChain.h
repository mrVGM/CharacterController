#pragma once

#include "CompositeTypeDef.h"
#include "CompositeValue.h"

#include "Job.h"

#include "d3dx12.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <string>

namespace rendering
{
    class DXSwapChainTypeDef : public ReferenceTypeDef
    {
        TYPE_DEF_BODY(DXSwapChainTypeDef)

    public:
		DXSwapChainTypeDef();
        virtual ~DXSwapChainTypeDef();

        void Construct(Value& value) const override;
    };

    class DXSwapChain : public ObjectValue
    {
		static const UINT FrameCount = 2;
		CD3DX12_VIEWPORT m_viewport;
		CD3DX12_RECT m_scissorRect;

		Microsoft::WRL::ComPtr<IDXGISwapChain3> m_swapChain;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
		UINT m_rtvDescriptorSize;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_renderTargets[FrameCount];

		void Create();


	public:
		DXSwapChain(const ReferenceTypeDef& typeDef);
		virtual ~DXSwapChain();

		void Load(jobs::Job* done);

		void Present();

		const CD3DX12_VIEWPORT& GetViewport() const;
		const CD3DX12_RECT& GetScissorRect() const;

		CD3DX12_CPU_DESCRIPTOR_HANDLE GetCurrentRTVDescriptor() const;
		ID3D12Resource* GetCurrentRenderTarget() const;

		UINT GetCurrentSwapChainIndex();
    };
}