#pragma once

#include "CompositeTypeDef.h"
#include "CompositeValue.h"

#include "Jobs.h"

#include "MultiLoader.h"

#include "d3dx12.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <string>

namespace rendering
{
    class DXFenceTypeDef : public ReferenceTypeDef
    {
        TYPE_DEF_BODY(DXFenceTypeDef)

    public:
        DXFenceTypeDef();
        virtual ~DXFenceTypeDef();

        void Construct(Value& value) const override;
    };

	class DXFence : public ObjectValue, public jobs::LoadingClass
	{
	private:
		int m_eventCounter = 0;
		Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
		void Create();

	protected:
		void LoadData(jobs::Job done) override;

	public:
		DXFence(const ReferenceTypeDef& typeDef);
		virtual ~DXFence();

		ID3D12Fence* GetFence() const;
	};
}