#include "SceneObject.h"

#include "ListDef.h"
#include "PrimitiveTypes.h"

#include "Mesh.h"

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
	m_meshList(ListDef::GetTypeDef(geo::MeshTypeDef::GetTypeDef()), this),
	m_meshDefList(SceneObjectTypeDef::GetTypeDef().m_meshList.GetType(), this)
{
}

scene::SceneObject::~SceneObject()
{
}

void scene::SceneObject::Load(jobs::Job* done)
{
	throw "Not Implemented!";
}
