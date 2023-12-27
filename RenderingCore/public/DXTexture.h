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
		void LoadData(jobs::Job* done) override;

	public:
		DXTexture(const ReferenceTypeDef& typeDef, const D3D12_RESOURCE_DESC& description);
		virtual ~DXTexture();

		void Load(jobs::Job* done);

		const D3D12_RESOURCE_DESC& GetTextureDescription() const;
		D3D12_RESOURCE_ALLOCATION_INFO GetTextureAllocationInfo() const;
		ID3D12Resource* GetTexture() const;

		static DXTexture* CreateDepthStencilTexture(const ReferenceTypeDef& typeDef, UINT width, UINT height);
		static DXTexture* CreateRenderTargetTexture(const ReferenceTypeDef& typeDef, UINT width, UINT height);
	};
}