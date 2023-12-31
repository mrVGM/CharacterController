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
	m_vertexWeightsMapBuffer(rendering::DXBufferTypeDef::GetTypeDef(), this)
{
}

runtime::MeshBuffers::~MeshBuffers()
{
}

void runtime::MeshBuffers::Load(geo::Mesh& mesh, jobs::Job* done)
{
	struct Context
	{
		Value m_vMutBuffer;
		Value m_iMutBuffer;

		Value m_weightsMapBuffer;
		Value m_weightsBuffer;

		int m_toLoad = 0;

		Context() :
			m_vMutBuffer(rendering::DXMutableBufferTypeDef::GetTypeDef(), nullptr),
			m_iMutBuffer(rendering::DXMutableBufferTypeDef::GetTypeDef(), nullptr),
			m_weightsMapBuffer(rendering::DXMutableBufferTypeDef::GetTypeDef(), nullptr),
			m_weightsBuffer(rendering::DXMutableBufferTypeDef::GetTypeDef(), nullptr)
		{
		}
	};
	Context* ctx = new Context();

	geo::Mesh* meshPtr = &mesh;

	auto buffLoaded = [=]() {
		using namespace rendering;

		--ctx->m_toLoad;
		if (ctx->m_toLoad > 0)
		{
			return;
		}

		m_vertexBuffer = ctx->m_vMutBuffer.GetValue<DXMutableBuffer*>()->m_buffer;
		m_indexBuffer = ctx->m_iMutBuffer.GetValue<DXMutableBuffer*>()->m_buffer;

		if (meshPtr->m_skinData.m_hasAnyData)
		{
			m_vertexWeightsMapBuffer = ctx->m_weightsMapBuffer.GetValue<DXMutableBuffer*>()->m_buffer;
			m_vertexWeightsBuffer = ctx->m_weightsBuffer.GetValue<DXMutableBuffer*>()->m_buffer;
		}

		delete ctx;

		jobs::RunSync(done);
	};

	jobs::Job* initMutBuffers = jobs::Job::CreateByLambda([=]() {
		using namespace rendering;

		DXMutableBufferTypeDef::GetTypeDef().Construct(ctx->m_vMutBuffer);
		DXMutableBufferTypeDef::GetTypeDef().Construct(ctx->m_iMutBuffer);

		DXMutableBuffer* vBuff = ctx->m_vMutBuffer.GetValue<rendering::DXMutableBuffer*>();
		DXMutableBuffer* iBuff = ctx->m_iMutBuffer.GetValue<rendering::DXMutableBuffer*>();
		
		vBuff->SetSizeAndStride(meshPtr->m_vertices.size() * sizeof(geo::MeshVertex), sizeof(geo::MeshVertex));
		iBuff->SetSizeAndStride(meshPtr->m_indices.size() * sizeof(int), sizeof(int));

		++ctx->m_toLoad;
		jobs::RunAsync(jobs::Job::CreateByLambda([=]() {

			vBuff->Load(jobs::Job::CreateByLambda([=]() {
				DXBuffer* upload = vBuff->m_uploadBuffer.GetValue<DXBuffer*>();
				void* data = upload->Map();
				meshPtr->InitVertexBuffer(data);
				upload->Unmap();

				vBuff->Upload(jobs::Job::CreateByLambda(buffLoaded));
			}));
			
		}));

		++ctx->m_toLoad;
		jobs::RunAsync(jobs::Job::CreateByLambda([=]() {

			iBuff->Load(jobs::Job::CreateByLambda([=]() {
				DXBuffer* upload = iBuff->m_uploadBuffer.GetValue<DXBuffer*>();
				void* data = upload->Map();
				meshPtr->InitIndexBuffer(data);
				upload->Unmap();

				iBuff->Upload(jobs::Job::CreateByLambda(buffLoaded));
			}));

		}));

		if (!meshPtr->m_skinData.m_hasAnyData)
		{
			return;
		}

		DXMutableBufferTypeDef::GetTypeDef().Construct(ctx->m_weightsBuffer);
		DXMutableBufferTypeDef::GetTypeDef().Construct(ctx->m_weightsMapBuffer);

		DXMutableBuffer* weightsBuff = ctx->m_weightsBuffer.GetValue<rendering::DXMutableBuffer*>();
		weightsBuff->SetSizeAndStride(meshPtr->m_skinData.m_vertexWeights.size() * sizeof(geo::Mesh::SkinData::VertexWeights), sizeof(geo::Mesh::SkinData::VertexWeights));

		DXMutableBuffer* weightsMapBuff = ctx->m_weightsMapBuffer.GetValue<rendering::DXMutableBuffer*>();
		weightsMapBuff->SetSizeAndStride(meshPtr->m_skinData.m_vertexToWeightsMap.size() * sizeof(int), sizeof(int));

		++ctx->m_toLoad;
		jobs::RunAsync(jobs::Job::CreateByLambda([=]() {

			weightsBuff->Load(jobs::Job::CreateByLambda([=]() {
				DXBuffer* upload = weightsBuff->m_uploadBuffer.GetValue<DXBuffer*>();
				void* data = upload->Map();
				geo::Mesh::SkinData::VertexWeights* weightsPtr = reinterpret_cast<geo::Mesh::SkinData::VertexWeights*>(data);
				for (auto it = meshPtr->m_skinData.m_vertexWeights.begin(); it != meshPtr->m_skinData.m_vertexWeights.end(); ++it)
				{
					*(weightsPtr++) = *it;
				}
				upload->Unmap();

				weightsBuff->Upload(jobs::Job::CreateByLambda(buffLoaded));
			}));

		}));

		++ctx->m_toLoad;
		jobs::RunAsync(jobs::Job::CreateByLambda([=]() {

			weightsMapBuff->Load(jobs::Job::CreateByLambda([=]() {
				DXBuffer* upload = weightsMapBuff->m_uploadBuffer.GetValue<DXBuffer*>();
				void* data = upload->Map();
				int* weightsMapPtr = reinterpret_cast<int*>(data);
				for (auto it = meshPtr->m_skinData.m_vertexToWeightsMap.begin(); it != meshPtr->m_skinData.m_vertexToWeightsMap.end(); ++it)
				{
					*(weightsMapPtr++) = *it;
				}
				upload->Unmap();

				weightsMapBuff->Upload(jobs::Job::CreateByLambda(buffLoaded));
			}));

		}));
	});

	jobs::Job* loadMesh = jobs::Job::CreateByLambda([=]() {
		meshPtr->Load(initMutBuffers);
	});

	jobs::RunAsync(loadMesh);
}
