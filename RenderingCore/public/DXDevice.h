#pragma once

#include "CompositeTypeDef.h"
#include "CompositeValue.h"

#include "d3dx12.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <string>

namespace rendering
{
    class DXDeviceTypeDef : public ReferenceTypeDef
    {
        TYPE_DEF_BODY(DXDeviceTypeDef)

    public:
        DXDeviceTypeDef();
        virtual ~DXDeviceTypeDef();

        void Construct(Value& value) const override;
    };

    class DXDevice : public ObjectValue
    {
        Microsoft::WRL::ComPtr<IDXGIFactory4> m_factory;
        Microsoft::WRL::ComPtr<ID3D12Device> m_device;

        void Create();

    public:
        DXDevice(const ReferenceTypeDef& typeDef);
        virtual ~DXDevice();

        ID3D12Device& GetDevice();
        IDXGIFactory4* GetFactory() const;
    };
}