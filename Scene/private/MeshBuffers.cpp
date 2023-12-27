#include "MeshBuffers.h"

#include "PrimitiveTypes.h"

#include "Mesh.h"

#include "DXBuffer.h"
#include "DXMutableBuffer.h"

#include "Jobs.h"

namespace
{
	BasicObjectContainer<scene::MeshBuffersTypeDef> m_meshBuffers;
}

const scene::MeshBuffersTypeDef& scene::MeshBuffersTypeDef::GetTypeDef()
{
	if (!m_meshBuffers.m_object)
	{
		m_meshBuffers.m_object = new MeshBuffersTypeDef();
	}

	return *m_meshBuffers.m_object;
}

scene::MeshBuffersTypeDef::MeshBuffersTypeDef() :
	ReferenceTypeDef(&ReferenceTypeDef::GetTypeDef(), "17752AAF-1089-4CF4-BDA9-EA66338F6778")
{
	m_name = "Mesh Buffers";
	m_category = "Scene";
}

scene::MeshBuffersTypeDef::~MeshBuffersTypeDef()
{
}

void scene::MeshBuffersTypeDef::Construct(Value& container) const
{
	MeshBuffers* meshBuffers = new MeshBuffers(*this);
	container.AssignObject(meshBuffers);
}

scene::MeshBuffers::MeshBuffers(const ReferenceTypeDef& typeDef) :
	ObjectValue(typeDef),
	m_vertexBuffer(rendering::DXBufferTypeDef::GetTypeDef(), this),
	m_indexBuffer(rendering::DXBufferTypeDef::GetTypeDef(), this)
{
}

scene::MeshBuffers::~MeshBuffers()
{
}

void scene::MeshBuffers::Load(geo::Mesh& mesh, jobs::Job* done)
{
	struct Context
	{
		Value m_vMutBuffer;
		Value m_iMutBuffer;

		int m_toLoad = 0;

		Context() :
			m_vMutBuffer(rendering::DXMutableBufferTypeDef::GetTypeDef(), nullptr),
			m_iMutBuffer(rendering::DXMutableBufferTypeDef::GetTypeDef(), nullptr)
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

		delete ctx;

		jobs::RunSync(done);
	};

	geo::Mesh* meshPtr = &mesh;
	jobs::Job* initMutBuffers = jobs::Job::CreateByLambda([=]() {
		using namespace rendering;

		DXMutableBufferTypeDef::GetTypeDef().Construct(ctx->m_vMutBuffer);
		DXMutableBufferTypeDef::GetTypeDef().Construct(ctx->m_iMutBuffer);

		DXMutableBuffer* vBuff = ctx->m_vMutBuffer.GetValue<rendering::DXMutableBuffer*>();
		DXMutableBuffer* iBuff = ctx->m_iMutBuffer.GetValue<rendering::DXMutableBuffer*>();
		
		vBuff->SetSizeAndStride(meshPtr->m_numVertices * sizeof(geo::MeshVertex), sizeof(geo::MeshVertex));
		iBuff->SetSizeAndStride(meshPtr->m_numIndices * sizeof(int), sizeof(int));

		ctx->m_toLoad = 2;

		jobs::RunAsync(jobs::Job::CreateByLambda([=]() {

			vBuff->Load(jobs::Job::CreateByLambda([=]() {
				DXBuffer* upload = vBuff->m_uploadBuffer.GetValue<DXBuffer*>();
				void* data = upload->Map();
				memcpy(data, meshPtr->m_vertices, upload->GetBufferSize());
				upload->Unmap();

				vBuff->Upload(jobs::Job::CreateByLambda(buffLoaded));
			}));
			
		}));

		jobs::RunAsync(jobs::Job::CreateByLambda([=]() {

			iBuff->Load(jobs::Job::CreateByLambda([=]() {
				DXBuffer* upload = iBuff->m_uploadBuffer.GetValue<DXBuffer*>();
				void* data = upload->Map();
				memcpy(data, meshPtr->m_indices, upload->GetBufferSize());
				upload->Unmap();

				iBuff->Upload(jobs::Job::CreateByLambda(buffLoaded));
			}));

		}));
	});

	jobs::Job* loadMesh = jobs::Job::CreateByLambda([=]() {
		meshPtr->Load(initMutBuffers);
	});

	jobs::RunAsync(loadMesh);
}
