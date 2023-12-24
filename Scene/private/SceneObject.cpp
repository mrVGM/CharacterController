#include "SceneObject.h"

#include "ListDef.h"
#include "PrimitiveTypes.h"
#include "ValueList.h"

#include "ObjectValueContainer.h"

#include "Mesh.h"
#include "Actor.h"

#include "Jobs.h"

#include "CoreUtils.h"

namespace
{
	BasicObjectContainer<scene::SceneObjectTypeDef> m_sceneObject;
}

const scene::SceneObjectTypeDef& scene::SceneObjectTypeDef::GetTypeDef()
{
	if (!m_sceneObject.m_object)
	{
		m_sceneObject.m_object = new SceneObjectTypeDef();
	}

	return *m_sceneObject.m_object;
}

scene::SceneObjectTypeDef::SceneObjectTypeDef() :
	ReferenceTypeDef(&ReferenceTypeDef::GetTypeDef(), "F5D74F81-F245-4D04-8E56-763D88D5169D"),
	m_meshList(
		"267F2995-4E66-4D09-83B4-C9F021BE63FA",
		ListDef::GetTypeDef(TypeTypeDef::GetTypeDef(geo::MeshTypeDef::GetTypeDef())))
{
	
	{
		m_meshList.m_name = "Mesh List";
		m_meshList.m_category = "Setup";
		m_meshList.m_getValue = [](CompositeValue* obj) -> Value& {
			scene::SceneObject* scene = static_cast<SceneObject*>(obj);
			return scene->m_meshDefList;
		};

		m_properties[m_meshList.GetId()] = &m_meshList;
	}

	m_name = "Scene Object";
	m_category = "Scene";
}

scene::SceneObjectTypeDef::~SceneObjectTypeDef()
{
}

void scene::SceneObjectTypeDef::Construct(Value& container) const
{
	SceneObject* sceneObject = new SceneObject(*this);
	container.AssignObject(sceneObject);
}

scene::SceneObject::SceneObject(const ReferenceTypeDef& typeDef) :
	ObjectValue(typeDef),
	m_actors(ListDef::GetTypeDef(ActorTypeDef::GetTypeDef()), this),
	m_meshDefList(SceneObjectTypeDef::GetTypeDef().m_meshList.GetType(), this)
{
}

scene::SceneObject::~SceneObject()
{
}

void scene::SceneObject::Load(jobs::Job* done)
{
	ValueList* meshDefs = m_meshDefList.GetValue<ValueList*>();
	ValueList* actorList = m_actors.GetValue<ValueList*>();

	struct Context
	{
		int m_toLoad = 0;
	};
	Context* ctx = new Context();

	auto actorLoaded = [=]() {
		--ctx->m_toLoad;
		if (ctx->m_toLoad > 0)
		{
			return;
		}

		delete ctx;

		jobs::RunSync(done);
	};

	auto loadActor = [=](Actor* actor) {
		return jobs::Job::CreateByLambda([=]() {
			actor->Load(jobs::Job::CreateByLambda(actorLoaded));
		});
	};

	jobs::Job* initActors = jobs::Job::CreateByLambda([=]() {

		for (auto it = meshDefs->GetIterator(); it; ++it)
		{
			const Value& cur = *it;
			geo::Mesh* curMesh = static_cast<geo::Mesh*>(ObjectValueContainer::GetObjectOfType(*cur.GetType<const TypeDef*>()));

			Value& actorVal = actorList->EmplaceBack();
			ActorTypeDef::GetTypeDef().Construct(actorVal);

			Actor* actor = actorVal.GetValue<Actor*>();

			actor->SetMesh(curMesh);

			++ctx->m_toLoad;
			jobs::RunAsync(loadActor(actor));
		}
	});

	jobs::RunSync(initActors);
}
