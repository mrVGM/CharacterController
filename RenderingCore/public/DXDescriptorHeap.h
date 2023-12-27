#pragma once

#include "CompositeTypeDef.h"
#include "CompositeValue.h"

#include "MultiLoader.h"

#include "Job.h"

#include "d3dx12.h"
#include <wrl.h>

namespace rendering
{
	class DXTexture;

    class DXDescriptorHeapTypeDef : public ReferenceTypeDef
    {
        TYPE_DEF_BODY(DXDescriptorHeapTypeDef)

    public:
		TypeProperty m_textures;

		DXDescriptorHeapTypeDef();
        virtual ~DXDescriptorHeapTypeDef();

        void Construct(Value& container) const override;
    };

	class DepthStencilDescriptorHeapTypeDef : public ReferenceTypeDef
	{
		TYPE_DEF_BODY(DepthStencilDescriptorHeapTypeDef)

	public:

		DepthStencilDescriptorHeapTypeDef();
		virtual ~DepthStencilDescriptorHeapTypeDef();

		void Construct(Value& container) const override;
	};

	class DXDescriptorHeap : public ObjectValue, public jobs::LoadingClass
	{
	private:
		jobs::MultiLoader m_loader;

	protected:
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_descriptorHeap;
		UINT m_descriptorSize = 0;


		Value m_textures;
		Value m_device;
		int m_size = 0;

		virtual void LoadData(jobs::Job* done) override;
		virtual void Init() = 0;

	public:
		Value m_textureDefs;

		DXDescriptorHeap(const ReferenceTypeDef& typeDef);
		virtual ~DXDescriptorHeap();

		void Load(jobs::Job* done);

		ID3D12DescriptorHeap* GetDescriptorHeap();
		D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle(UINT index);
	};

	class DepthStencilDescriptorHeap : public DXDescriptorHeap
	{
	protected:
		void Init() override;

	public:
		DepthStencilDescriptorHeap(const ReferenceTypeDef& typeDef);
		virtual ~DepthStencilDescriptorHeap();
	};
}