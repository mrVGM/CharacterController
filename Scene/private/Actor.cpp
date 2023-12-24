#include "Actor.h"

#include "PrimitiveTypes.h"

#include "Mesh.h"
#include "MeshBuffers.h"

#include "Jobs.h"

namespace
{
	BasicObjectContainer<scene::ActorTypeDef> m_actorTypeDef;
}

const scene::ActorTypeDef& scene::ActorTypeDef::GetTypeDef()
{
	if (!m_actorTypeDef.m_object)
	{
		m_actorTypeDef.m_object = new ActorTypeDef();
	}

	return *m_actorTypeDef.m_object;
}

scene::ActorTypeDef::ActorTypeDef() :
	ReferenceTypeDef(&ReferenceTypeDef::GetTypeDef(), "9CFFA9B0-3542-4509-81CD-387C496ADAFF"),
	m_mesh("6B501787-EFF6-4EDF-85A4-571428A47071", geo::MeshTypeDef::GetTypeDef())
{
	m_name = "Actor";
	m_category = "Scene";
}

scene::ActorTypeDef::~ActorTypeDef()
{
}

void scene::ActorTypeDef::Construct(Value& container) const
{
	Actor* actor = new Actor(*this);
	container.AssignObject(actor);
}

scene::Actor::Actor(const ReferenceTypeDef& typeDef) :
	ObjectValue(typeDef),
	m_mesh(geo::MeshTypeDef::GetTypeDef(), this)
{
}

scene::Actor::~Actor()
{
}

void scene::Actor::SetMesh(geo::Mesh* mesh)
{
	m_mesh.AssignObject(mesh);
}

void scene::Actor::Load(jobs::Job* done)
{
	geo::Mesh* mesh = m_mesh.GetValue<geo::Mesh*>();
	MeshBuffers* meshBuffers = mesh->m_buffers.GetValue<MeshBuffers*>();

	if (meshBuffers)
	{
		jobs::RunSync(done);
		return;
	}

	jobs::Job* initMeshBuffers = jobs::Job::CreateByLambda([=]() {
		MeshBuffersTypeDef::GetTypeDef().Construct(mesh->m_buffers);
		MeshBuffers* meshBuffers = mesh->m_buffers.GetValue<MeshBuffers*>();

		jobs::RunAsync(jobs::Job::CreateByLambda([=]() {
			meshBuffers->Load(*mesh, done);
		}));
	});

	jobs::RunSync(initMeshBuffers);
}
