#pragma once

#include "CompositeTypeDef.h"
#include "CompositeValue.h"

#include "Job.h"
#include "DXHeap.h"

#include "MultiLoader.h"

#include "d3dx12.h"
#include <wrl.h>

namespace rendering
{
    class DXTextureTypeDef : public ReferenceTypeDef
    {
        TYPE_DEF_BODY(DXTextureTypeDef)

    public:
		DXTextureTypeDef();
        virtual ~DXTextureTypeDef();

        void Construct(Value& container) const override;
    };

	class DXTexture : public ObjectValue, public jobs::LoadingClass
	{
	private:
		Value m_heap;
		Value m_device;

		jobs::MultiLoader m_loader;

		D3D12_RESOURCE_DESC m_description;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_texture;

		void Place(DXHeap& heap, UINT64 heapOffset);

	protected:
		virtual void LoadData(jobs::Job* done) override;

	public:
		DXTexture(const ReferenceTypeDef& typeDef);

		virtual ~DXTexture();

		void SetDescription(const D3D12_RESOURCE_DESC& description);
		void Load(jobs::Job* done);

		const D3D12_RESOURCE_DESC& GetTextureDescription() const;
		D3D12_RESOURCE_ALLOCATION_INFO GetTextureAllocationInfo() const;
		ID3D12Resource* GetTexture() const;

		static D3D12_RESOURCE_DESC CreateDepthStencilTextureDescription(UINT width, UINT height);
		static D3D12_RESOURCE_DESC CreateRenderTargetTextureDescription(UINT width, UINT height);
	};
}