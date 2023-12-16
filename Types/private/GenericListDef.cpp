#include "GenericListDef.h"

#include "CompositeValue.h"

namespace
{
	const char* m_genericListDefId = "B5883DED-257C-4E35-846C-DB5BB7E11289";

	BasicObjectContainer<GenericListDef> m_genericListDef;
}

const GenericListDef& GenericListDef::GetTypeDef()
{
	if (!m_genericListDef.m_object)
	{
		m_genericListDef.m_object = new GenericListDef();
	}
	return *m_genericListDef.m_object;
}

GenericListDef::GenericListDef() :
	TypeDef(&ValueTypeDef::GetTypeDef(), m_genericListDefId)
{
	m_name = "Generic List";
	m_category = "Composite";
}

void GenericListDef::GetReflectionData(json_parser::JSONValue& outData) const
{
	using namespace json_parser;

	TypeDef::GetReflectionData(outData);
	auto& map = outData.GetAsObj();

	map["hint"] = JSONValue("list");
}

