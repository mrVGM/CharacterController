#include "UnlitMaterial.h"

#include "PrimitiveTypes.h"

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
	BasicObjectContainer<rendering::unlit_rp::UnlitMaterialTypeDef> m_unlitMaterial;
}


const rendering::unlit_rp::UnlitMaterialTypeDef& rendering::unlit_rp::UnlitMaterialTypeDef::GetTypeDef()
{
	if (!m_unlitMaterial.m_object)
	{
		m_unlitMaterial.m_object = new rendering::unlit_rp::UnlitMaterialTypeDef();
	}

	return *m_unlitMaterial.m_object;
}

rendering::unlit_rp::UnlitMaterialTypeDef::UnlitMaterialTypeDef() :
	ReferenceTypeDef(&materials::MaterialTypeDef::GetTypeDef(), "FE33ED22-B48B-4567-B4DF-575CF941D787"),
    m_rtDescHeap("20D42103-7616-4545-9123-B858E8F38050", TypeTypeDef::GetTypeDef(RenderTargetDescriptorHeapTypeDef::GetTypeDef())),
    m_skeletalMeshVertexShader("24133A1B-721D-4B89-A796-4609ACACAD1C", TypeTypeDef::GetTypeDef(DXVertexShaderTypeDef::GetTypeDef()))
{
    {
        m_rtDescHeap.m_name = "RT Heap";
        m_rtDescHeap.m_category = "Setup";
        m_rtDescHeap.m_getValue = [](CompositeValue* obj) -> Value& {
            UnlitMaterial* mat = static_cast<UnlitMaterial*>(obj);
            return mat->m_rtDescHeapDef;
        };
        m_properties[m_rtDescHeap.GetId()] = &m_rtDescHeap;
    }

    {
        m_skeletalMeshVertexShader.m_name = "Skeletal Mesh Vertex Shader";
        m_skeletalMeshVertexShader.m_category = "Setup";
        m_skeletalMeshVertexShader.m_getValue = [](CompositeValue* obj) -> Value& {
            UnlitMaterial* mat = static_cast<UnlitMaterial*>(obj);
            return mat->m_skeletalMeshVertexShaderDef;
        };
        m_properties[m_skeletalMeshVertexShader.GetId()] = &m_skeletalMeshVertexShader;
    }

	m_name = "Unlit Material";
	m_category = "Unlit RP";
}

rendering::unlit_rp::UnlitMaterialTypeDef::~UnlitMaterialTypeDef()
{
}

void rendering::unlit_rp::UnlitMaterialTypeDef::Construct(Value& container) const
{
	UnlitMaterial* mat = new UnlitMaterial(*this);
	container.AssignObject(mat);
}

rendering::unlit_rp::UnlitMaterial::UnlitMaterial(const ReferenceTypeDef& typeDef) :
    materials::Material(typeDef),

    m_rtDescHeapDef(UnlitMaterialTypeDef::GetTypeDef().m_rtDescHeap.GetType(), this),
    m_rtDescHeap(RenderTargetDescriptorHeapTypeDef::GetTypeDef(), this),

    m_skeletalMeshVertexShaderDef(UnlitMaterialTypeDef::GetTypeDef().m_skeletalMeshVertexShader.GetType(), this),
    m_skeletalMeshVertexShader(DXVertexShaderTypeDef::GetTypeDef(), this),

	m_dsDescriptorHeap(DXDescriptorHeapTypeDef::GetTypeDef(), this),
    m_camBuffer(render_pass::CameraBufferTypeDef::GetTypeDef(), this)
{
}

rendering::unlit_rp::UnlitMaterial::~UnlitMaterial()
{
}

void rendering::unlit_rp::UnlitMaterial::CreatePipelineStateAndRootSignatureForStaticMesh()
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

        // Allow input layout and deny uneccessary access to certain pipeline stages.
        D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
        CD3DX12_ROOT_PARAMETER1 rootParameters[2];
        rootParameters[0].InitAsConstantBufferView(0, 0);
        rootParameters[1].InitAsConstantBufferView(1, 0);

        rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);

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

        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

        psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 1;

        psoDesc.RTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;

        psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
        psoDesc.SampleDesc.Count = 1;
        THROW_ERROR(
            device->GetDevice().CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState)),
            "Can't create Graphics Pipeline State!")
    }
}

void rendering::unlit_rp::UnlitMaterial::CreatePipelineStateAndRootSignatureForSkeletalMesh()
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

        // Allow input layout and deny uneccessary access to certain pipeline stages.
        D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
        CD3DX12_ROOT_PARAMETER1 rootParameters[5];
        rootParameters[0].InitAsConstantBufferView(0, 0);
        rootParameters[1].InitAsConstantBufferView(1, 0);
        rootParameters[2].InitAsShaderResourceView(0, 0);
        rootParameters[3].InitAsShaderResourceView(1, 0);
        rootParameters[4].InitAsShaderResourceView(2, 0);

        rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);

        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> error;
        THROW_ERROR(
            D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error),
            "Can't serialize a root signature!")

        THROW_ERROR(
            device->GetDevice().CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignatureSkeletalMesh)),
            "Can't create a root signature!")
    }


    // Create the pipeline state, which includes compiling and loading shaders.
    {
        // Define the vertex input layout.
        const D3D12_INPUT_ELEMENT_DESC* inputElementDescs = nullptr;
        unsigned int inputElementsCount = 0;

        materials::Get3DSkeletalMeshMaterialInputLayout(inputElementDescs, inputElementsCount);

        // Describe and create the graphics pipeline state object (PSO).
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.InputLayout = { inputElementDescs, inputElementsCount };
        psoDesc.pRootSignature = m_rootSignatureSkeletalMesh.Get();
        psoDesc.VS = CD3DX12_SHADER_BYTECODE(m_skeletalMeshVertexShader.GetValue<DXShader*>()->GetCompiledShader());
        psoDesc.PS = CD3DX12_SHADER_BYTECODE(m_pixelShader.GetValue<DXShader*>()->GetCompiledShader());
        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

        //psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;

        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

        psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 1;

        psoDesc.RTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;

        psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
        psoDesc.SampleDesc.Count = 1;
        THROW_ERROR(
            device->GetDevice().CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineStateSkeletalMesh)),
            "Can't create Graphics Pipeline State!")
    }
}

void rendering::unlit_rp::UnlitMaterial::GenerateCommandList(
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

    commandList->SetGraphicsRootSignature(m_rootSignature.Get());
    commandList->SetGraphicsRootConstantBufferView(0, m_camBuffer.GetValue<DXMutableBuffer*>()->m_buffer.GetValue<DXBuffer*>()->GetBuffer()->GetGPUVirtualAddress());
    commandList->SetGraphicsRootConstantBufferView(1, instanceBuffer.GetBuffer()->GetGPUVirtualAddress());

    commandList->RSSetViewports(1, &swapChain->GetViewport());
    commandList->RSSetScissorRects(1, &swapChain->GetScissorRect());

    D3D12_CPU_DESCRIPTOR_HANDLE dsHandle = m_dsDescriptorHeap.GetValue<DXDescriptorHeap*>()->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
    D3D12_CPU_DESCRIPTOR_HANDLE handles[] = { m_rtDescHeap.GetValue<DXDescriptorHeap*>()->GetDescriptorHandle(0) };
    commandList->OMSetRenderTargets(_countof(handles), handles, FALSE, &dsHandle);
    
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


void rendering::unlit_rp::UnlitMaterial::GenerateCommandList(
    const DXBuffer& vertexBuffer,
    const DXBuffer& indexBuffer,
    const DXBuffer& instanceBuffer,

    const DXBuffer& weightsIdBuffer,
    const DXBuffer& weightsBuffer,

    const DXBuffer& bindShapeBuffer,
    const DXBuffer& poseBuffer,

    UINT startIndex,
    UINT indexCount,
    ID3D12CommandAllocator* commandAllocator,
    ID3D12GraphicsCommandList* commandList)
{
    THROW_ERROR(
        commandList->Reset(commandAllocator, m_pipelineStateSkeletalMesh.Get()),
        "Can't reset Command List!")

    DXSwapChain* swapChain = m_swapChain.GetValue<DXSwapChain*>();

    commandList->SetGraphicsRootSignature(m_rootSignatureSkeletalMesh.Get());
    commandList->SetGraphicsRootConstantBufferView(0, m_camBuffer.GetValue<DXMutableBuffer*>()->m_buffer.GetValue<DXBuffer*>()->GetBuffer()->GetGPUVirtualAddress());
    commandList->SetGraphicsRootConstantBufferView(1, instanceBuffer.GetBuffer()->GetGPUVirtualAddress());

    commandList->SetGraphicsRootShaderResourceView(2, weightsBuffer.GetBuffer()->GetGPUVirtualAddress());
    commandList->SetGraphicsRootShaderResourceView(3, bindShapeBuffer.GetBuffer()->GetGPUVirtualAddress());
    commandList->SetGraphicsRootShaderResourceView(4, poseBuffer.GetBuffer()->GetGPUVirtualAddress());

    commandList->RSSetViewports(1, &swapChain->GetViewport());
    commandList->RSSetScissorRects(1, &swapChain->GetScissorRect());

    D3D12_CPU_DESCRIPTOR_HANDLE dsHandle = m_dsDescriptorHeap.GetValue<DXDescriptorHeap*>()->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
    D3D12_CPU_DESCRIPTOR_HANDLE handles[] = { m_rtDescHeap.GetValue<DXDescriptorHeap*>()->GetDescriptorHandle(0) };
    commandList->OMSetRenderTargets(_countof(handles), handles, FALSE, &dsHandle);

    D3D12_VERTEX_BUFFER_VIEW vertexBufferViews[2];
    D3D12_VERTEX_BUFFER_VIEW& realVertexBufferView = vertexBufferViews[0];
    realVertexBufferView.BufferLocation = vertexBuffer.GetBuffer()->GetGPUVirtualAddress();
    realVertexBufferView.StrideInBytes = vertexBuffer.GetStride();
    realVertexBufferView.SizeInBytes = vertexBuffer.GetBufferSize();

    D3D12_VERTEX_BUFFER_VIEW& wightsIdView = vertexBufferViews[1];
    wightsIdView.BufferLocation = weightsIdBuffer.GetBuffer()->GetGPUVirtualAddress();
    wightsIdView.StrideInBytes = weightsIdBuffer.GetStride();
    wightsIdView.SizeInBytes = weightsIdBuffer.GetBufferSize();

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

const Value& rendering::unlit_rp::UnlitMaterial::GetRTHeap() const
{
    return m_rtDescHeap;
}

void rendering::unlit_rp::UnlitMaterial::LoadData(jobs::Job* done)
{
    struct Context
    {
        int m_loading = 0;
    };
    Context* ctx = new Context();

    auto itemLoaded = [=]() {
        --ctx->m_loading;
        if (ctx->m_loading > 0)
        {
            return;
        }

        delete ctx;

        CreatePipelineStateAndRootSignatureForStaticMesh();
        CreatePipelineStateAndRootSignatureForSkeletalMesh();
        jobs::RunSync(done);
    };


    jobs::Job* parentLoaded = jobs::Job::CreateByLambda([=]() {
        DXDescriptorHeap* dsHeap = m_dsDescriptorHeap.GetValue<DXDescriptorHeap*>();
        DXDescriptorHeap* rtHeap = m_rtDescHeap.GetValue<DXDescriptorHeap*>();
        DXShader* skeletalMeshVertexShader = m_skeletalMeshVertexShader.GetValue<DXShader*>();

        ++ctx->m_loading;
        jobs::RunAsync(jobs::Job::CreateByLambda([=]() {
            dsHeap->Load(jobs::Job::CreateByLambda(itemLoaded));
        }));

        ++ctx->m_loading;
        jobs::RunAsync(jobs::Job::CreateByLambda([=]() {
            rtHeap->Load(jobs::Job::CreateByLambda(itemLoaded));
        }));

        ++ctx->m_loading;
        jobs::RunAsync(jobs::Job::CreateByLambda([=]() {
            skeletalMeshVertexShader->Load(jobs::Job::CreateByLambda(itemLoaded));
        }));
    });

    jobs::Job* loadParent = jobs::Job::CreateByLambda([=]() {
        materials::Material::LoadData(parentLoaded);
    });

    jobs::Job* init = jobs::Job::CreateByLambda([=]() {
        ObjectValueContainer::GetObjectOfType(render_pass::CameraBufferTypeDef::GetTypeDef(), m_camBuffer);
        ObjectValueContainer::GetObjectOfType(DepthStencilDescriptorHeapTypeDef::GetTypeDef(), m_dsDescriptorHeap);
        ObjectValueContainer::GetObjectOfType(*m_rtDescHeapDef.GetType<const TypeDef*>(), m_rtDescHeap);
        ObjectValueContainer::GetObjectOfType(*m_skeletalMeshVertexShaderDef.GetType<const TypeDef*>(), m_skeletalMeshVertexShader);

        jobs::RunAsync(loadParent);
    });

    jobs::RunSync(init);
}

#undef THROW_ERROR