#include "SceneObject.h"

#include "SceneActor.h"
#include "AssetTypeDef.h"

#include "ListDef.h"
#include "PrimitiveTypes.h"
#include "ValueList.h"

#include "ObjectValueContainer.h"

#include "Mesh.h"
#include "Actor.h"

#include "Jobs.h"

#include "MathStructs.h"

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
		"5E4B2953-2059-42F3-929E-312F74D77930",
		ListDef::GetTypeDef(SceneActorTypeDef::GetTypeDef()))
{
	{
		m_actorList.m_name = "Actor List";
		m_actorList.m_category = "Setup";
		m_actorList.m_getValue = [](CompositeValue* obj) -> Value& {
			scene::SceneObject* scene = static_cast<SceneObject*>(obj);
			return scene->m_actorList;
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
	m_actorList(SceneObjectTypeDef::GetTypeDef().m_actorList.GetType(), this)
{
}

scene::SceneObject::~SceneObject()
{
}

void scene::SceneObject::Load(jobs::Job* done)
{
	ValueList* sceneActorList = m_actorList.GetValue<ValueList*>();
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

		for (auto it = sceneActorList->GetIterator(); it; ++it)
		{
			const Value& cur = *it;
			const SceneActorValue* sceneActorValue = cur.GetValue<SceneActorValue*>();

			Value& actorVal = actorList->EmplaceBack();
			const AssetTypeDef* actorAsset = static_cast<const AssetTypeDef*>(sceneActorValue->m_actorDef.GetType<const TypeDef*>());
			actorAsset->Construct(actorVal);

			runtime::Actor* actor = actorVal.GetValue<runtime::Actor*>();

			{
				common::TransformValue* transform = sceneActorValue->m_transform.GetValue<common::TransformValue*>();
				common::Vector3Value* position = transform->m_position.GetValue<common::Vector3Value*>();
				common::Vector3Value* rotation = transform->m_rotation.GetValue<common::Vector3Value*>();
				common::Vector3Value* scale = transform->m_scale.GetValue<common::Vector3Value*>();

				actor->m_curTransform.m_position = math::Vector3{ position->m_x.Get<float>(), position->m_y.Get<float>(), position->m_z.Get<float>() };
				actor->m_curTransform.m_rotation = math::Vector3{ rotation->m_x.Get<float>(), rotation->m_y.Get<float>(), rotation->m_z.Get<float>() };
				actor->m_curTransform.m_scale = math::Vector3{ scale->m_x.Get<float>(), scale->m_y.Get<float>(), scale->m_z.Get<float>() };
			}

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