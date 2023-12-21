#include "DXDevice.h"

#define THROW_ERROR(hRes, error) \
if (FAILED(hRes)) {\
    throw error;\
}

namespace
{
    BasicObjectContainer<rendering::DXDeviceTypeDef> m_deviceTypeDef;
}

rendering::DXDeviceTypeDef::DXDeviceTypeDef() :
    ReferenceTypeDef(&ReferenceTypeDef::GetTypeDef(), "E2067F1A-67AF-4681-AF92-AB0EFF4A6226")
{
    m_name = "Device";
    m_category = "Rendering";
}

rendering::DXDeviceTypeDef::~DXDeviceTypeDef()
{
}

const rendering::DXDeviceTypeDef& rendering::DXDeviceTypeDef::GetTypeDef()
{
    if (!m_deviceTypeDef.m_object)
    {
        m_deviceTypeDef.m_object = new DXDeviceTypeDef();
    }

    return *m_deviceTypeDef.m_object;
}

void rendering::DXDevice::Create()
{
    using Microsoft::WRL::ComPtr;

    UINT dxgiFactoryFlags = 0;

#if DEBUG
    // Enable the debug layer (requires the Graphics Tools "optional feature").
    // NOTE: Enabling the debug layer after device creation will invalidate the active device.
    {
        ComPtr<ID3D12Debug> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
        {
            debugController->EnableDebugLayer();

            // Enable additional debug layers.
            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
        }
    }
#endif

    THROW_ERROR(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_factory)), "Can't create DXGIFactoty!")

    {
        THROW_ERROR(D3D12CreateDevice(
            nullptr,
            D3D_FEATURE_LEVEL_11_0,
            IID_PPV_ARGS(&m_device)
        ), "Can't Create device");
    }
}


ID3D12Device& rendering::DXDevice::GetDevice()
{
    return *m_device.Get();
}

IDXGIFactory4* rendering::DXDevice::GetFactory() const
{
    return m_factory.Get();
}

rendering::DXDevice::DXDevice(const ReferenceTypeDef& typeDef) :
    ObjectValue(typeDef)
{
    Create();
}

rendering::DXDevice::~DXDevice()
{
}

#undef THROW_ERROR

