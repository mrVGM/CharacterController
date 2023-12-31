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
	m_actorList(
		"267F2995-4E66-4D09-83B4-C9F021BE63FA",
		ListDef::GetTypeDef(TypeTypeDef::GetTypeDef(runtime::ActorTypeDef::GetTypeDef())))
{
	
	{
		m_actorList.m_name = "Actor List";
		m_actorList.m_category = "Setup";
		m_actorList.m_getValue = [](CompositeValue* obj) -> Value& {
			scene::SceneObject* scene = static_cast<SceneObject*>(obj);
			return scene->m_actorDefList;
		};

		m_properties[m_actorList.GetId()] = &m_actorList;
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
	m_actors(ListDef::GetTypeDef(runtime::ActorTypeDef::GetTypeDef()), this),
	m_actorDefList(SceneObjectTypeDef::GetTypeDef().m_actorList.GetType(), this)
{
}

scene::SceneObject::~SceneObject()
{
}

void scene::SceneObject::Load(jobs::Job* done)
{
	ValueList* actorDefs = m_actorDefList.GetValue<ValueList*>();
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

	auto loadActor = [=](runtime::Actor* actor) {
		return jobs::Job::CreateByLambda([=]() {
			actor->Load(jobs::Job::CreateByLambda(actorLoaded));
		});
	};

	jobs::Job* initActors = jobs::Job::CreateByLambda([=]() {

		for (auto it = actorDefs->GetIterator(); it; ++it)
		{
			const Value& cur = *it;
			runtime::Actor* curActor = static_cast<runtime::Actor*>(ObjectValueContainer::GetObjectOfType(*cur.GetType<const TypeDef*>()));

			Value& actorVal = actorList->EmplaceBack();
			actorVal.AssignObject(curActor);

			runtime::Actor* actor = actorVal.GetValue<runtime::Actor*>();

			++ctx->m_toLoad;
			jobs::RunAsync(loadActor(actor));
		}
	});

	jobs::RunSync(initActors);
}


Value& scene::SceneObject::GetActors()
{
	return m_actors;
}