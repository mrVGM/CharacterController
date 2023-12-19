#include "Mesh.h"

#include "PrimitiveTypes.h"

#include "Jobs.h"

#include "Files.h"
#include "XMLReader.h"

namespace
{
	BasicObjectContainer<geo::MeshTypeDef> m_meshTypeDef;
}

geo::MeshTypeDef::MeshTypeDef() :
	ReferenceTypeDef(&ReferenceTypeDef::GetTypeDef(), "D963BDD2-9E53-466B-BCAF-F3FB50434050"),
	m_colladaFile("D9F8FC86-02D0-4EAF-A73E-8616AB554825", StringTypeDef::GetTypeDef())
{
	{
		m_colladaFile.m_name = "Collada File";
		m_colladaFile.m_category = "Setup";
		m_colladaFile.m_getValue = [](CompositeValue* obj) -> Value& {
			Mesh* mesh = static_cast<Mesh*>(obj);
			return mesh->m_colladaFile;
		};
		m_properties[m_colladaFile.GetId()] = &m_colladaFile;
	}

	m_name = "Mesh";
	m_category = "Geometry";
}

void geo::MeshTypeDef::Construct(Value& container) const
{
	Mesh* mesh = new Mesh(*this, nullptr);
	container.AssignObject(mesh);
}

const geo::MeshTypeDef& geo::MeshTypeDef::GetTypeDef()
{
	if (!m_meshTypeDef.m_object)
	{
		m_meshTypeDef.m_object = new MeshTypeDef();
	}

	return *m_meshTypeDef.m_object;
}

geo::Mesh::Mesh(const CompositeTypeDef& type, const CompositeValue* outer) :
	ObjectValue(type, outer),
	m_colladaFile(MeshTypeDef::GetTypeDef().m_colladaFile.GetType(), this)
{
}

void geo::Mesh::Load(jobs::Job* done)
{
	std::string colladaFile = "Geometry\\" + m_colladaFile.Get<std::string>();

	std::string contents;
	files::ReadTextFile(colladaFile, contents);

	xml_reader::XMLTree tree;
	xml_reader::ReadXML(contents, tree);

	jobs::RunSync(done);
}
