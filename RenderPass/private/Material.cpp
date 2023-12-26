#include "Material.h"

#include "PrimitiveTypes.h"
#include "ObjectValueContainer.h"

#include "Jobs.h"

#include "DXShader.h"

#include "CoreUtils.h"

namespace
{
	BasicObjectContainer<rendering::render_pass::MaterialTypeDef> m_material;
}


#define THROW_ERROR(hRes, error) \
if (FAILED(hRes)) {\
    throw error;\
}

const rendering::render_pass::MaterialTypeDef& rendering::render_pass::MaterialTypeDef::GetTypeDef()
{
	if (!m_material.m_object)
	{
		m_material.m_object = new rendering::render_pass::MaterialTypeDef();
	}

	return *m_material.m_object;
}

rendering::render_pass::MaterialTypeDef::MaterialTypeDef() :
	ReferenceTypeDef(&ReferenceTypeDef::GetTypeDef(), "88865622-C8C0-4807-9A04-031CAFD12150"),
	m_vertexShader("4152C1E7-1CC2-4901-BDCA-8A25D985E03C", TypeTypeDef::GetTypeDef(DXVertexShaderTypeDef::GetTypeDef())),
	m_pixelShader("4508B9D7-D184-442A-85EB-1B2DDA1556AC", TypeTypeDef::GetTypeDef(DXPixelShaderTypeDef::GetTypeDef()))
{
	{
		m_vertexShader.m_name = "Vertex Shader";
		m_vertexShader.m_category = "Setup";
		m_vertexShader.m_getValue = [](CompositeValue* obj) -> Value& {
			Material* mat = static_cast<Material*>(obj);
			return mat->m_vertexShaderDef;
		};
		m_properties[m_vertexShader.GetId()] = &m_vertexShader;
	}

	{
		m_pixelShader.m_name = "Pixel Shader";
		m_pixelShader.m_category = "Setup";
		m_pixelShader.m_getValue = [](CompositeValue* obj) -> Value& {
			Material* mat = static_cast<Material*>(obj);
			return mat->m_pixelShaderDef;
		};
		m_properties[m_pixelShader.GetId()] = &m_pixelShader;
	}

	m_name = "Material";
	m_category = "Render Pass";
}

rendering::render_pass::MaterialTypeDef::~MaterialTypeDef()
{
}

void rendering::render_pass::MaterialTypeDef::Construct(Value& container) const
{
	Material* mat = new Material(*this);
	container.AssignObject(mat);
}


void rendering::render_pass::Material::CreatePipelineStateAndRootSignatureForStaticMesh()
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
        CD3DX12_ROOT_PARAMETER1 rootParameters[1];
        rootParameters[0].InitAsConstantBufferView(0, 0);

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

        core::utils::Get3DMaterialInputLayout(inputElementDescs, inputElementsCount);

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

        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

        psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
        psoDesc.SampleDesc.Count = 1;
        THROW_ERROR(
            device->GetDevice().CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState)),
            "Can't create Graphics Pipeline State!")
    }
}

rendering::render_pass::Material::Material(const ReferenceTypeDef& typeDef) :
	ObjectValue(typeDef),
	m_vertexShader(DXVertexShaderTypeDef::GetTypeDef(), this),
	m_pixelShader(DXPixelShaderTypeDef::GetTypeDef(), this),
	m_device(DXDeviceTypeDef::GetTypeDef(), this),
    m_swapChain(DXSwapChainTypeDef::GetTypeDef(), this),
	
	m_vertexShaderDef(MaterialTypeDef::GetTypeDef().m_vertexShader.GetType(), this),
	m_pixelShaderDef(MaterialTypeDef::GetTypeDef().m_pixelShader.GetType(), this)
{
}

rendering::render_pass::Material::~Material()
{
}

void rendering::render_pass::Material::GenerateCommandList(
    const DXBuffer& vertexBuffer,
    const DXBuffer& indexBuffer,
    const DXBuffer& instanceBuffer,
    UINT startIndex,
    UINT indexCount,
    ID3D12GraphicsCommandList* commandList)
{
}

void rendering::render_pass::Material::Load(jobs::Job* done)
{
    struct Context
    {
        int m_toLoad = 0;
    };
    Context* ctx = new Context{ 2 };

    auto getVS = [=]() {
        return m_vertexShader.GetValue<DXShader*>();
    };

    auto getPS = [=]() {
        return m_pixelShader.GetValue<DXShader*>();
    };

    auto shaderLoaded = [=]() {
        --ctx->m_toLoad;
        if (ctx->m_toLoad > 0)
        {
            return;
        }
        delete ctx;

        CreatePipelineStateAndRootSignatureForStaticMesh();

        jobs::RunSync(done);
    };

    jobs::Job* init = jobs::Job::CreateByLambda([=]() {
        m_device.AssignObject(core::utils::GetDevice());
        m_swapChain.AssignObject(core::utils::GetSwapChain());
        m_vertexShader.AssignObject(ObjectValueContainer::GetObjectOfType(*m_vertexShaderDef.GetType<const TypeDef*>()));
        m_pixelShader.AssignObject(ObjectValueContainer::GetObjectOfType(*m_pixelShaderDef.GetType<const TypeDef*>()));

        jobs::RunAsync(jobs::Job::CreateByLambda([=]() {
            getVS()->Load(jobs::Job::CreateByLambda([=]() {
                jobs::RunSync(jobs::Job::CreateByLambda(shaderLoaded));
            }));
        }));

        jobs::RunAsync(jobs::Job::CreateByLambda([=]() {
            getPS()->Load(jobs::Job::CreateByLambda([=]() {
                jobs::RunSync(jobs::Job::CreateByLambda(shaderLoaded));
            }));
        }));
    });

    jobs::RunSync(init);
}

#undef THROW_ERROR