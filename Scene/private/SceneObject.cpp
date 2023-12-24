#include "SceneObject.h"

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
	ReferenceTypeDef(&ReferenceTypeDef::GetTypeDef(), "F5D74F81-F245-4D04-8E56-763D88D5169D")
{
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
	ObjectValue(typeDef)
{
}

scene::SceneObject::~SceneObject()
{
}

void scene::SceneObject::Load(jobs::Job* done)
{
	throw "Not Implemented!";
}
