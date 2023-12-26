#include "UnlitMaterial.h"

#include "PrimitiveTypes.h"
#include "ObjectValueContainer.h"

#include "Jobs.h"

#include "DXMutableBuffer.h"

#include "CameraBuffer.h"

#include "ObjectValueContainer.h"

#include "CoreUtils.h"

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
	ReferenceTypeDef(&render_pass::MaterialTypeDef::GetTypeDef(), "FE33ED22-B48B-4567-B4DF-575CF941D787")
{
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
	render_pass::Material(typeDef),
    m_camBuffer(render_pass::CameraBufferTypeDef::GetTypeDef(), this)
{
}

rendering::unlit_rp::UnlitMaterial::~UnlitMaterial()
{
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

    ID3D12Resource* curRT = m_swapChain.GetValue<DXSwapChain*>()->GetCurrentRenderTarget();

    commandList->SetGraphicsRootSignature(m_rootSignature.Get());
    commandList->SetGraphicsRootConstantBufferView(0, m_camBuffer.GetValue<DXMutableBuffer*>()->m_buffer.GetValue<DXBuffer*>()->GetBuffer()->GetGPUVirtualAddress());

    commandList->RSSetViewports(1, &swapChain->GetViewport());
    commandList->RSSetScissorRects(1, &swapChain->GetScissorRect());

    //D3D12_CPU_DESCRIPTOR_HANDLE dsHandle = m_depthStencilDescriptorHeap->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
    D3D12_CPU_DESCRIPTOR_HANDLE handles[] = { swapChain->GetCurrentRTVDescriptor() };
    //commandList->OMSetRenderTargets(_countof(handles), handles, FALSE, &dsHandle);
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

void rendering::unlit_rp::UnlitMaterial::Load(jobs::Job* done)
{
    jobs::Job* parentLoaded = jobs::Job::CreateByLambda([=]() {
        ObjectValue* camBuff = ObjectValueContainer::GetObjectOfType(render_pass::CameraBufferTypeDef::GetTypeDef());
        m_camBuffer.AssignObject(camBuff);

        jobs::RunSync(done);
    });

    jobs::Job* loadParent = jobs::Job::CreateByLambda([=]() {
        render_pass::Material::Load(parentLoaded);
    });

    jobs::RunAsync(loadParent);
}

#undef THROW_ERROR