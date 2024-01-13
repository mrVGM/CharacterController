#include "SceneActor.h"

#include "PrimitiveTypes.h"
#include "MathStructs.h"

#include "Actor.h"

namespace
{
	BasicObjectContainer<scene::SceneActorTypeDef> m_sceneActor;
}

const scene::SceneActorTypeDef& scene::SceneActorTypeDef::GetTypeDef()
{
	if (!m_sceneActor.m_object)
	{
		m_sceneActor.m_object = new scene::SceneActorTypeDef();
	}

	return *m_sceneActor.m_object;
}

scene::SceneActorTypeDef::SceneActorTypeDef() :
	ValueTypeDef(&ValueTypeDef::GetTypeDef(), "BD092EB2-905B-4C30-A9D8-7E96C644D2F3"),
	m_transform("833DE71F-3C95-41E1-BB02-F80DFEDFF36A", common::TransformTypeDef::GetTypeDef()),
	m_actor("59D36C1D-465C-4293-8F7D-F80CFBEF9DE3", TypeTypeDef::GetTypeDef(runtime::MeshActorTypeDef::GetTypeDef()))
{
	{
		m_transform.m_name = "Transform";
		m_transform.m_category = "";
		m_transform.m_getValue = [](CompositeValue* value) -> Value& {
			SceneActorValue* sceneActor = static_cast<SceneActorValue*>(value);
			return sceneActor->m_transform;
		};
		m_properties[m_transform.GetId()] = &m_transform;
	}

	{
		m_actor.m_name = "Actor";
		m_actor.m_category = "";
		m_actor.m_getValue = [](CompositeValue* value) -> Value& {
			SceneActorValue* sceneActor = static_cast<SceneActorValue*>(value);
			return sceneActor->m_actorDef;
		};
		m_properties[m_actor.GetId()] = &m_actor;
	}

	m_name = "Scene Actor";
	m_category = "Scene";
}

scene::SceneActorTypeDef::~SceneActorTypeDef()
{
}

void scene::SceneActorTypeDef::Construct(Value& container) const
{
	SceneActorValue* tmp = new SceneActorValue(container.m_outer);
	container.m_payload = tmp;
}

scene::SceneActorValue::SceneActorValue(size_t outer) :
	CopyValue(SceneActorTypeDef::GetTypeDef(), outer),
	m_transform(SceneActorTypeDef::GetTypeDef().m_transform.GetType(), this),
	m_actorDef(SceneActorTypeDef::GetTypeDef().m_actor.GetType(), this)
{
}

scene::SceneActorValue::~SceneActorValue()
{
}

void scene::SceneActorValue::Copy(const CopyValue& src)
{
	const scene::SceneActorValue& other = static_cast<const scene::SceneActorValue&>(src);
	*this = other;
}
