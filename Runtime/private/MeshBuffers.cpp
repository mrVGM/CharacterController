#include "MeshBuffers.h"

#include "PrimitiveTypes.h"

#include "Mesh.h"

#include "DXBuffer.h"
#include "DXMutableBuffer.h"

#include "Jobs.h"

namespace
{
	BasicObjectContainer<runtime::MeshBuffersTypeDef> m_meshBuffers;
}

const runtime::MeshBuffersTypeDef& runtime::MeshBuffersTypeDef::GetTypeDef()
{
	if (!m_meshBuffers.m_object)
	{
		m_meshBuffers.m_object = new MeshBuffersTypeDef();
	}

	return *m_meshBuffers.m_object;
}

runtime::MeshBuffersTypeDef::MeshBuffersTypeDef() :
	ReferenceTypeDef(&ReferenceTypeDef::GetTypeDef(), "17752AAF-1089-4CF4-BDA9-EA66338F6778")
{
	m_name = "Mesh Buffers";
	m_category = "Scene";
}

runtime::MeshBuffersTypeDef::~MeshBuffersTypeDef()
{
}

void runtime::MeshBuffersTypeDef::Construct(Value& container) const
{
	MeshBuffers* meshBuffers = new MeshBuffers(*this);
	container.AssignObject(meshBuffers);
}

runtime::MeshBuffers::MeshBuffers(const ReferenceTypeDef& typeDef) :
	ObjectValue(typeDef),
	m_vertexBuffer(rendering::DXBufferTypeDef::GetTypeDef(), this),
	m_indexBuffer(rendering::DXBufferTypeDef::GetTypeDef(), this),
	m_vertexWeightsBuffer(rendering::DXBufferTypeDef::GetTypeDef(), this),
	m_vertexWeightsMapBuffer(rendering::DXBufferTypeDef::GetTypeDef(), this),
	m_bindShapeBuffer(rendering::DXBufferTypeDef::GetTypeDef(), this)
{
}

runtime::MeshBuffers::~MeshBuffers()
{
}

void runtime::MeshBuffers::Load(geo::Mesh& mesh, jobs::Job done)
{
	geo::Mesh* meshPtr = &mesh;

	jobs::Job setMesh = [=]() {
		if (this->m_mesh)
		{
			jobs::RunSync(done);
			return;
		}

		m_mesh = meshPtr;
		jobs::RunAsync([=]() {
			LoadData(done);
		});
	};

	jobs::RunSync(setMesh);
}

void runtime::MeshBuffers::LoadData(jobs::Job done)
{
	struct Context
	{
		Value m_vMutBuffer;
		Value m_iMutBuffer;

		Value m_weightsMapBuffer;
		Value m_weightsBuffer;
		Value m_bindShapeBuffer;

		int m_toLoad = 0;

		Context() :
			m_vMutBuffer(rendering::DXMutableBufferTypeDef::GetTypeDef(), nullptr),
			m_iMutBuffer(rendering::DXMutableBufferTypeDef::GetTypeDef(), nullptr),
			m_weightsMapBuffer(rendering::DXMutableBufferTypeDef::GetTypeDef(), nullptr),
			m_weightsBuffer(rendering::DXMutableBufferTypeDef::GetTypeDef(), nullptr),
			m_bindShapeBuffer(rendering::DXMutableBufferTypeDef::GetTypeDef(), nullptr)
		{
		}
	};
	Context* ctx = new Context();

	auto buffLoaded = [=]() {
		using namespace rendering;

		--ctx->m_toLoad;
		if (ctx->m_toLoad > 0)
		{
			return;
		}

		m_vertexBuffer = ctx->m_vMutBuffer.GetValue<DXMutableBuffer*>()->m_buffer;
		m_indexBuffer = ctx->m_iMutBuffer.GetValue<DXMutableBuffer*>()->m_buffer;

		if (m_mesh->m_skinData.m_hasAnyData)
		{
			m_vertexWeightsMapBuffer = ctx->m_weightsMapBuffer.GetValue<DXMutableBuffer*>()->m_buffer;
			m_vertexWeightsBuffer = ctx->m_weightsBuffer.GetValue<DXMutableBuffer*>()->m_buffer;
			m_bindShapeBuffer = ctx->m_bindShapeBuffer.GetValue<DXMutableBuffer*>()->m_buffer;
		}

		delete ctx;

		jobs::RunSync(done);
	};

	jobs::Job initMutBuffers = [=]() {
		using namespace rendering;

		DXMutableBufferTypeDef::GetTypeDef().Construct(ctx->m_vMutBuffer);
		DXMutableBufferTypeDef::GetTypeDef().Construct(ctx->m_iMutBuffer);

		DXMutableBuffer* vBuff = ctx->m_vMutBuffer.GetValue<rendering::DXMutableBuffer*>();
		DXMutableBuffer* iBuff = ctx->m_iMutBuffer.GetValue<rendering::DXMutableBuffer*>();
		
		vBuff->SetSizeAndStride(m_mesh->m_vertices.size() * sizeof(geo::MeshVertex), sizeof(geo::MeshVertex));
		iBuff->SetSizeAndStride(m_mesh->m_indices.size() * sizeof(int), sizeof(int));

		++ctx->m_toLoad;
		jobs::RunAsync([=]() {

			vBuff->Load([=]() {
				DXBuffer* upload = vBuff->m_uploadBuffer.GetValue<DXBuffer*>();
				void* data = upload->Map();
				m_mesh->InitVertexBuffer(data);
				upload->Unmap();

				vBuff->Upload(buffLoaded);
			});
			
		});

		++ctx->m_toLoad;
		jobs::RunAsync([=]() {

			iBuff->Load([=]() {
				DXBuffer* upload = iBuff->m_uploadBuffer.GetValue<DXBuffer*>();
				void* data = upload->Map();
				m_mesh->InitIndexBuffer(data);
				upload->Unmap();

				iBuff->Upload(buffLoaded);
			});

		});

		if (!m_mesh->m_skinData.m_hasAnyData)
		{
			return;
		}

		DXMutableBufferTypeDef::GetTypeDef().Construct(ctx->m_weightsBuffer);
		DXMutableBufferTypeDef::GetTypeDef().Construct(ctx->m_weightsMapBuffer);
		DXMutableBufferTypeDef::GetTypeDef().Construct(ctx->m_bindShapeBuffer);

		DXMutableBuffer* weightsBuff = ctx->m_weightsBuffer.GetValue<rendering::DXMutableBuffer*>();
		weightsBuff->SetSizeAndStride(m_mesh->m_skinData.m_vertexWeights.size() * sizeof(geo::Mesh::SkinData::VertexWeights), sizeof(geo::Mesh::SkinData::VertexWeights));

		DXMutableBuffer* weightsMapBuff = ctx->m_weightsMapBuffer.GetValue<rendering::DXMutableBuffer*>();
		weightsMapBuff->SetSizeAndStride(m_mesh->m_skinData.m_vertexToWeightsMap.size() * sizeof(int), sizeof(int));

		DXMutableBuffer* bindShapeBuffer = ctx->m_bindShapeBuffer.GetValue<DXMutableBuffer*>();
		bindShapeBuffer->SetSizeAndStride((m_mesh->m_skinData.m_invBindMatrices.size() + 1) * sizeof(math::Matrix), sizeof(math::Matrix));

		++ctx->m_toLoad;
		jobs::RunAsync([=]() {

			weightsBuff->Load([=]() {
				DXBuffer* upload = weightsBuff->m_uploadBuffer.GetValue<DXBuffer*>();
				void* data = upload->Map();
				geo::Mesh::SkinData::VertexWeights* weightsPtr = reinterpret_cast<geo::Mesh::SkinData::VertexWeights*>(data);
				for (auto it = m_mesh->m_skinData.m_vertexWeights.begin(); it != m_mesh->m_skinData.m_vertexWeights.end(); ++it)
				{
					*(weightsPtr++) = *it;
				}
				upload->Unmap();

				weightsBuff->Upload(buffLoaded);
			});

		});

		++ctx->m_toLoad;
		jobs::RunAsync([=]() {

			weightsMapBuff->Load([=]() {
				DXBuffer* upload = weightsMapBuff->m_uploadBuffer.GetValue<DXBuffer*>();
				void* data = upload->Map();
				int* weightsMapPtr = reinterpret_cast<int*>(data);
				for (auto it = m_mesh->m_skinData.m_vertexToWeightsMap.begin(); it != m_mesh->m_skinData.m_vertexToWeightsMap.end(); ++it)
				{
					*(weightsMapPtr++) = *it;
				}
				upload->Unmap();

				weightsMapBuff->Upload(buffLoaded);
			});

		});

		++ctx->m_toLoad;
		jobs::RunAsync([=]() {

			bindShapeBuffer->Load([=]() {
				DXBuffer* upload = bindShapeBuffer->m_uploadBuffer.GetValue<DXBuffer*>();
				void* data = upload->Map();
				math::Matrix* bindShapePtr = reinterpret_cast<math::Matrix*>(data);

				{
					math::Matrix matTmp = m_mesh->m_skinData.m_bindShapeMatrix.Transpose();
					*(bindShapePtr++) = matTmp;
				}

				for (auto it = m_mesh->m_skinData.m_invBindMatrices.begin(); it != m_mesh->m_skinData.m_invBindMatrices.end(); ++it)
				{
					math::Matrix matTmp = (*it).Transpose();
					*(bindShapePtr++) = matTmp;
				}
				upload->Unmap();

				bindShapeBuffer->Upload(buffLoaded);
			});

		});
	};

	jobs::Job loadMesh = [=]() {
		m_mesh->Load(initMutBuffers);
	};

	jobs::RunAsync(loadMesh);
}
