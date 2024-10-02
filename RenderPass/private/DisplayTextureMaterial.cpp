#include "DisplayTextureMaterial.h"

#include "PrimitiveTypes.h"
#include "ObjectValueContainer.h"

#include "Jobs.h"

#include "DXMutableBuffer.h"

#include "CameraBuffer.h"

#include "DXShader.h"

#include "DXDescriptorHeap.h"
#include "DXDevice.h"
#include "DXSwapChain.h"

#include "VertexLayouts.h"

#include "ObjectValueContainer.h"

#define THROW_ERROR(hRes, error) \
if (FAILED(hRes)) {\
    throw error;\
}

namespace
{
	BasicObjectContainer<rendering::render_pass::DisplayTextureMaterialTypeDef> m_displayTextureMaterial;
}


const rendering::render_pass::DisplayTextureMaterialTypeDef& rendering::render_pass::DisplayTextureMaterialTypeDef::GetTypeDef()
{
	if (!m_displayTextureMaterial.m_object)
	{
		m_displayTextureMaterial.m_object = new rendering::render_pass::DisplayTextureMaterialTypeDef();
	}

	return *m_displayTextureMaterial.m_object;
}

rendering::render_pass::DisplayTextureMaterialTypeDef::DisplayTextureMaterialTypeDef() :
	ReferenceTypeDef(&materials::MaterialTypeDef::GetTypeDef(), "A30008B2-5172-4C76-8983-65036701D061"),
    m_texDescriptorHeap("3EAA65D3-D125-4FDE-87B9-E0EE0ECE3EC1", TypeTypeDef::GetTypeDef(ShaderResourceDescriptorHeapTypeDef::GetTypeDef()))
{
    {
        m_texDescriptorHeap.m_name = "Descriptor Heap";
        m_texDescriptorHeap.m_category = "Setup";
        m_texDescriptorHeap.m_getValue = [](CompositeValue* obj) -> Value& {
            DisplayTextureMaterial* mat = static_cast<DisplayTextureMaterial*>(obj);
            return mat->m_texDescriptorHeapDef;
        };
        m_properties[m_texDescriptorHeap.GetId()] = &m_texDescriptorHeap;
    }

	m_name = "Display Texture Material";
	m_category = "Render Pass";
}

rendering::render_pass::DisplayTextureMaterialTypeDef::~DisplayTextureMaterialTypeDef()
{
}

void rendering::render_pass::DisplayTextureMaterialTypeDef::Construct(Value& container) const
{
    DisplayTextureMaterial* mat = new DisplayTextureMaterial(*this);
	container.AssignObject(mat);
}

rendering::render_pass::DisplayTextureMaterial::DisplayTextureMaterial(const ReferenceTypeDef& typeDef) :
	materials::Material(typeDef),
    m_texDescriptorHeapDef(DisplayTextureMaterialTypeDef::GetTypeDef().m_texDescriptorHeap.GetType(), this),
    m_texDescriptorHeap(ShaderResourceDescriptorHeapTypeDef::GetTypeDef(), this)
{
}

rendering::render_pass::DisplayTextureMaterial::~DisplayTextureMaterial()
{
}

void rendering::render_pass::DisplayTextureMaterial::CreatePipelineStateAndRootSignatureForStaticMesh()
{
    DXDevice* device = m_device.GetValue<DXDevice*>();

    using Microsoft::WRL::ComPtr;
    {
        D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};

        // This is the highest version the sample supports. If CheckFeatureSupport succeeds, the HighestVersion returned will not be greater than this.
        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

        if (FAILED(device->GetDevice().CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData)))) {
            featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
        }

        D3D12_STATIC_SAMPLER_DESC sampler = {};
        sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
        sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        sampler.MipLODBias = 0;
        sampler.MaxAnisotropy = 0;
        sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
        sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
        sampler.MinLOD = 0.0f;
        sampler.MaxLOD = D3D12_FLOAT32_MAX;
        sampler.ShaderRegister = 0;
        sampler.RegisterSpace = 0;
        sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

        // Allow input layout and deny uneccessary access to certain pipeline stages.
        D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;

        CD3DX12_DESCRIPTOR_RANGE1 ranges[1];
        ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0);
        CD3DX12_ROOT_PARAMETER1 rootParameters[1];
        rootParameters[0].InitAsDescriptorTable(1, ranges, D3D12_SHADER_VISIBILITY_PIXEL);

        rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 1, &sampler, rootSignatureFlags);

        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> error;
        THROW_ERROR(
            D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error),
            "Can't serialize a root signature!")

        THROW_ERROR(
            device->GetDevice().CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)),
            "Can't create a root signature!")
    }


    // Create the pipeline state, which includes compiling and loading shaders.
    {
        // Define the vertex input layout.
        const D3D12_INPUT_ELEMENT_DESC* inputElementDescs = nullptr;
        unsigned int inputElementsCount = 0;

        materials::Get3DMaterialInputLayout(inputElementDescs, inputElementsCount);

        // Describe and create the graphics pipeline state object (PSO).
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.InputLayout = { inputElementDescs, inputElementsCount };
        psoDesc.pRootSignature = m_rootSignature.Get();
        psoDesc.VS = CD3DX12_SHADER_BYTECODE(m_vertexShader.GetValue<DXShader*>()->GetCompiledShader());
        psoDesc.PS = CD3DX12_SHADER_BYTECODE(m_pixelShader.GetValue<DXShader*>()->GetCompiledShader());
        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

        {
            psoDesc.BlendState.RenderTarget[0].BlendEnable = TRUE;
            psoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND::D3D12_BLEND_SRC_ALPHA;
            psoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND::D3D12_BLEND_INV_SRC_ALPHA;
            psoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP::D3D12_BLEND_OP_ADD;
            psoDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP::D3D12_BLEND_OP_ADD;
            psoDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND::D3D12_BLEND_ONE;
            psoDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND::D3D12_BLEND_ZERO;
            psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = 0x0f;
        }

        psoDesc.DepthStencilState.DepthEnable = false;
        psoDesc.DepthStencilState.StencilEnable = false;

        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 1;

        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

        psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
        psoDesc.SampleDesc.Count = 1;
        THROW_ERROR(
            device->GetDevice().CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState)),
            "Can't create Graphics Pipeline State!")
    }
}

void rendering::render_pass::DisplayTextureMaterial::GenerateCommandList(
    const DXBuffer& vertexBuffer,
    const DXBuffer& indexBuffer,
    const DXBuffer& instanceBuffer,
    UINT startIndex,
    UINT indexCount,
    ID3D12CommandAllocator* commandAllocator,
    ID3D12GraphicsCommandList* commandList)
{
    THROW_ERROR(
        commandList->Reset(commandAllocator, m_pipelineState.Get()),
        "Can't reset Command List!")

    DXSwapChain* swapChain = m_swapChain.GetValue<DXSwapChain*>();

    ID3D12Resource* curRT = m_swapChain.GetValue<DXSwapChain*>()->GetCurrentRenderTarget();

    commandList->SetGraphicsRootSignature(m_rootSignature.Get());
    
    DXDescriptorHeap* descHeap = m_texDescriptorHeap.GetValue<DXDescriptorHeap*>();
    ID3D12DescriptorHeap* descHeaps[] = { descHeap->GetDescriptorHeap() };
    commandList->SetDescriptorHeaps(_countof(descHeaps), descHeaps);
    commandList->SetGraphicsRootDescriptorTable(0, descHeap->GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());

    commandList->RSSetViewports(1, &swapChain->GetViewport());
    commandList->RSSetScissorRects(1, &swapChain->GetScissorRect());

    D3D12_CPU_DESCRIPTOR_HANDLE handles[] = { swapChain->GetCurrentRTVDescriptor() };
    commandList->OMSetRenderTargets(_countof(handles), handles, FALSE, nullptr);
    
    D3D12_VERTEX_BUFFER_VIEW vertexBufferViews[1];
    D3D12_VERTEX_BUFFER_VIEW& realVertexBufferView = vertexBufferViews[0];
    realVertexBufferView.BufferLocation = vertexBuffer.GetBuffer()->GetGPUVirtualAddress();
    realVertexBufferView.StrideInBytes = vertexBuffer.GetStride();
    realVertexBufferView.SizeInBytes = vertexBuffer.GetBufferSize();

    D3D12_INDEX_BUFFER_VIEW indexBufferView;
    indexBufferView.BufferLocation = indexBuffer.GetBuffer()->GetGPUVirtualAddress();
    indexBufferView.Format = DXGI_FORMAT_R32_UINT;
    indexBufferView.SizeInBytes = indexBuffer.GetBufferSize();

    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    commandList->IASetVertexBuffers(0, _countof(vertexBufferViews), vertexBufferViews);
    commandList->IASetIndexBuffer(&indexBufferView);

    int numInstances = instanceBuffer.GetBufferSize() / instanceBuffer.GetStride();
    commandList->DrawIndexedInstanced(
        indexCount,
        1,
        startIndex,
        0,
        0
    );

    THROW_ERROR(
        commandList->Close(),
        "Can't close Command List!")
}

void rendering::render_pass::DisplayTextureMaterial::LoadData(jobs::Job done)
{
    struct Context
    {
        int m_loading = 0;
    };
    Context* ctx = new Context();

    auto getDescHeap = [=]() {
        return m_texDescriptorHeap.GetValue<DXDescriptorHeap*>();
    };

    auto itemLoaded = [=]() {
        --ctx->m_loading;
        if (ctx->m_loading > 0)
        {
            return;
        }
        delete ctx;

        CreatePipelineStateAndRootSignatureForStaticMesh();
        jobs::RunSync(done);
    };

    jobs::Job loadParent = [=]() {
        materials::Material::LoadData(itemLoaded);
    };

    jobs::Job loadDescriptorHeap = [=]() {
        DXDescriptorHeap* heap = getDescHeap();
        heap->Load(itemLoaded);
    };

    jobs::Job init = [=]() {
        ObjectValueContainer::GetObjectOfType(*m_texDescriptorHeapDef.GetType<const TypeDef*>(), m_texDescriptorHeap);

        ctx->m_loading = 2;
        jobs::RunAsync(loadParent);
        jobs::RunAsync(loadDescriptorHeap);
    };

    jobs::RunSync(init);
}

#undef THROW_ERROR