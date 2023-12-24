#include "GeometryAppEntry.h"

#include "AppEntry.h"

#include "Jobs.h"
#include "Job.h"

#include "Mesh.h"

#include "ObjectValueContainer.h"

namespace
{
	BasicObjectContainer<geo::GeometryAppEntryTypeDef> m_geometryAppEntryTypeDef;
}

geo::GeometryAppEntryTypeDef::GeometryAppEntryTypeDef() :
	ReferenceTypeDef(&app::AppEntryTypeDef::GetTypeDef(), "F2E86D51-BDD2-4570-8338-9A0B73BF5C00")
{
	m_name = "Geometry App Entry";
	m_category = "Geometry";
}

geo::GeometryAppEntryTypeDef::~GeometryAppEntryTypeDef()
{
}

void geo::GeometryAppEntryTypeDef::Construct(Value& container) const
{
	GeometryAppEntryObj* appEntry = new GeometryAppEntryObj(*this);
	container.AssignObject(appEntry);
}

const geo::GeometryAppEntryTypeDef& geo::GeometryAppEntryTypeDef::GetTypeDef()
{
	if (!m_geometryAppEntryTypeDef.m_object)
	{
		m_geometryAppEntryTypeDef.m_object = new GeometryAppEntryTypeDef();
	}

	return *m_geometryAppEntryTypeDef.m_object;
}

geo::GeometryAppEntryObj::GeometryAppEntryObj(const ReferenceTypeDef& typeDef) :
	AppEntryObj(typeDef)
{
}

geo::GeometryAppEntryObj::~GeometryAppEntryObj()
{
}

void geo::GeometryAppEntryObj::Boot()
{
	class LoadDone : public jobs::Job
	{
	public:
		void Do() override
		{
		}
	};

	class LoadMesh : public jobs::Job
	{
	public:
		void Do() override
		{
			ObjectValueContainer& container = ObjectValueContainer::GetContainer();

			std::list<ObjectValue*> tmp;
			container.GetObjectsOfType(MeshTypeDef::GetTypeDef(), tmp);

			Mesh* mesh = static_cast<Mesh*>(tmp.front());

			mesh->Load(new LoadDone());
		}
	};

	jobs::RunSync(new LoadMesh());
}
