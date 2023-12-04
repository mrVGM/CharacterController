#include "ListTypeDef.h"

#include "GenericListTypeDef.h"

namespace
{
	std::string m_listDefId = "9CAA7EDD-2377-40B1-8F12-F81544B25E5F";
}

void ListTypeDef::GetTypeKey(json_parser::JSONValue& outTypeKey) const
{
	GetKey(m_templateType, outTypeKey);
}

void ListTypeDef::GetKey(const TypeDef& templateType, json_parser::JSONValue& outTypeKey)
{
	using namespace json_parser;
	TypeDef::GetDefaultTypeKey(m_listDefId, outTypeKey);

	auto& map = outTypeKey.GetAsObj();
	map["template"] = JSONValue(templateType.GetId());
}

const ListTypeDef& ListTypeDef::GetTypeDef(const TypeDef& templateType)
{
	using namespace json_parser;
	
	JSONValue tmp;
	GetKey(templateType, tmp);

	const TypeDefsMap& defsMap = GetDefsMap();
	auto it = defsMap.find(tmp.ToString(false));

	if (it != defsMap.end())
	{
		return *it->second;
	}

	ListTypeDef* def = new ListTypeDef(templateType);
	return *def;
}

ListTypeDef::ListTypeDef(const TypeDef& templateType) :
	ValueTypeDef(&GenericListTypeDef::GetTypeDef(), m_listDefId),
	m_templateType(templateType)
{
	m_isGenerated = true;
}
