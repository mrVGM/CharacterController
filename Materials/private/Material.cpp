#include "Material.h"

#include "PrimitiveTypes.h"
#include "ObjectValueContainer.h"

#include "Jobs.h"

#include "DXShader.h"

#include "DXDevice.h"
#include "DXSwapChain.h"

namespace
{
	BasicObjectContainer<rendering::materials::MaterialTypeDef> m_material;
}


#define THROW_ERROR(hRes, error) \
if (FAILED(hRes)) {\
    throw error;\
}

const rendering::materials::MaterialTypeDef& rendering::materials::MaterialTypeDef::GetTypeDef()
{
	if (!m_material.m_object)
	{
		m_material.m_object = new rendering::materials::MaterialTypeDef();
	}

	return *m_material.m_object;
}

rendering::materials::MaterialTypeDef::MaterialTypeDef() :
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

rendering::materials::MaterialTypeDef::~MaterialTypeDef()
{
}

void rendering::materials::MaterialTypeDef::Construct(Value& container) const
{
	Material* mat = new Material(*this);
	container.AssignObject(mat);
}

rendering::materials::Material::Material(const ReferenceTypeDef& typeDef) :
	ObjectValue(typeDef),
	m_vertexShader(DXVertexShaderTypeDef::GetTypeDef(), this),
	m_pixelShader(DXPixelShaderTypeDef::GetTypeDef(), this),
	m_device(DXDeviceTypeDef::GetTypeDef(), this),
    m_swapChain(DXSwapChainTypeDef::GetTypeDef(), this),
	
	m_vertexShaderDef(MaterialTypeDef::GetTypeDef().m_vertexShader.GetType(), this),
	m_pixelShaderDef(MaterialTypeDef::GetTypeDef().m_pixelShader.GetType(), this)
{
}

rendering::materials::Material::~Material()
{
}

void rendering::materials::Material::GenerateCommandList(
    const DXBuffer& vertexBuffer,
    const DXBuffer& indexBuffer,
    const DXBuffer& instanceBuffer,
    UINT startIndex,
    UINT indexCount,
    ID3D12CommandAllocator* commandAllocator,
    ID3D12GraphicsCommandList* commandList)
{
}

void rendering::materials::Material::GenerateCommandList(
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
}

void rendering::materials::Material::LoadData(jobs::Job* done)
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

        jobs::RunSync(done);
    };

    jobs::Job* init = jobs::Job::CreateByLambda([=]() {
		m_device.AssignObject(ObjectValueContainer::GetObjectOfType(DXDeviceTypeDef::GetTypeDef()));
		m_swapChain.AssignObject(ObjectValueContainer::GetObjectOfType(DXSwapChainTypeDef::GetTypeDef()));
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