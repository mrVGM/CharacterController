#include "DXSwapChain.h"

#include "RenderWindow.h"

#include "Jobs.h"

#include "CoreUtils.h"

#define THROW_ERROR(hRes, error) \
if (FAILED(hRes)) {\
    throw error;\
}

namespace
{
    BasicObjectContainer<rendering::DXSwapChainTypeDef> m_swapChain;
}


rendering::DXSwapChainTypeDef::DXSwapChainTypeDef() :
    ReferenceTypeDef(&ReferenceTypeDef::GetTypeDef(), "BAAA0222-EFEE-476B-BDB6-43C97EC9741E")
{
    m_name = "Swap Chain";
    m_category = "Rendering";
}

rendering::DXSwapChainTypeDef::~DXSwapChainTypeDef()
{
}

void rendering::DXSwapChainTypeDef::Construct(Value& value) const
{
    DXSwapChain* swapChain = new DXSwapChain(*this);
    value.AssignObject(swapChain);
}

const rendering::DXSwapChainTypeDef& rendering::DXSwapChainTypeDef::GetTypeDef()
{
    if (!m_swapChain.m_object)
    {
        m_swapChain.m_object = new DXSwapChainTypeDef();
    }

    return *m_swapChain.m_object;
}

void rendering::DXSwapChain::Create()
{
    using Microsoft::WRL::ComPtr;

    WindowObj* window = core::utils::GetWindow();
    if (!window)
    {
        throw "No Window found!";
    }

    DXDevice* device = core::utils::GetDevice();
    if (!device)
    {
        throw "No Device found!";
    }

    DXCommandQueue* commandQueue = core::utils::GetCommandQueue();
    if (!commandQueue)
    {
        throw "No Command Queue found!";
    }

    int width = window->m_width.Get<int>();
    int height = window->m_height.Get<int>();
    m_frameIndex = 0;
    m_viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height));
    m_scissorRect = CD3DX12_RECT(0, 0, static_cast<LONG>(width), static_cast<LONG>(height));
    m_rtvDescriptorSize = 0;

    // Describe and create the swap chain.
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = FrameCount;
    swapChainDesc.Width = width;
    swapChainDesc.Height = height;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;

    IDXGIFactory4* factory = device->GetFactory();
    ComPtr<IDXGISwapChain1> swapChain;
    THROW_ERROR(factory->CreateSwapChainForHwnd(
        commandQueue->GetCommandQueue(),        // Swap chain needs the queue so that it can force a flush on it.
        window->m_hwnd,
        &swapChainDesc,
        nullptr,
        nullptr,
        &swapChain
    ), "Can't Create Swap Chain")

        // This sample does not support fullscreen transitions.
        THROW_ERROR(
            factory->MakeWindowAssociation(window->m_hwnd, DXGI_MWA_NO_ALT_ENTER),
            "Can't Associate to Window!")

        THROW_ERROR(swapChain.As(&m_swapChain), "Can't cast to swap chain!")
        m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

    // Create descriptor heaps.
    {
        // Describe and create a render target view (RTV) descriptor heap.
        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
        rtvHeapDesc.NumDescriptors = FrameCount;
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        THROW_ERROR(
            device->GetDevice().CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)),
            "Can't create a descriptor heap!")

            m_rtvDescriptorSize = device->GetDevice().GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    }

    // Create frame resources.
    {
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

        // Create a RTV for each frame.
        for (UINT n = 0; n < FrameCount; n++)
        {
            THROW_ERROR(
                m_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n])),
                "Can't get buffer from the Swap Chain!")

                device->GetDevice().CreateRenderTargetView(m_renderTargets[n].Get(), nullptr, rtvHandle);
            rtvHandle.Offset(1, m_rtvDescriptorSize);
        }
    }
}

void rendering::DXSwapChain::Present()
{
    THROW_ERROR(m_swapChain->Present(1, 0),
        "Can't present Swap Chain!")
}

void rendering::DXSwapChain::UpdateCurrentFrameIndex()
{
    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
}


CD3DX12_CPU_DESCRIPTOR_HANDLE rendering::DXSwapChain::GetCurrentRTVDescriptor() const
{
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);
}

ID3D12Resource* rendering::DXSwapChain::GetCurrentRenderTarget() const
{
    return m_renderTargets[m_frameIndex].Get();
}

UINT rendering::DXSwapChain::GetCurrentSwapChainIndex()
{
    return m_swapChain->GetCurrentBackBufferIndex();
}

rendering::DXSwapChain::DXSwapChain(const ReferenceTypeDef& typeDef) :
    ObjectValue(typeDef)
{
}

rendering::DXSwapChain::~DXSwapChain()
{
}

void rendering::DXSwapChain::Load(jobs::Job* done)
{
    struct Context
    {
        DXSwapChain* m_self = nullptr;
        jobs::Job* m_done = nullptr;
    };

    Context ctx{ this, done };

    class LoadJob : public jobs::Job
    {
    private:
        Context m_ctx;
    public:
        LoadJob(const Context& ctx) :
            m_ctx(ctx)
        {
        }

        void Do() override
        {
            m_ctx.m_self->Create();

            jobs::RunSync(m_ctx.m_done);
        }
    };


    jobs::RunSync(new LoadJob(ctx));
}

const CD3DX12_VIEWPORT& rendering::DXSwapChain::GetViewport() const
{
    return m_viewport;
}

const CD3DX12_RECT& rendering::DXSwapChain::GetScissorRect() const
{
    return m_scissorRect;
}


#undef THROW_ERROR