#include "GenericListDef.h"

#include "CompositeValue.h"

namespace
{
	std::string m_listDefId = "61BE566E-F055-4537-BCBD-2E1A7335EB55";

	GenericListDef m_genericListDef;
}

const GenericListDef& GenericListDef::GetTypeDef()
{
	return m_genericListDef;
}

GenericListDef::GenericListDef() :
	TypeDef(&ValueTypeDef::GetTypeDef(), "B5883DED-257C-4E35-846C-DB5BB7E11289")
{
}

void GenericListDef::GetReflectionData(json_parser::JSONValue& outData)
{
	using namespace json_parser;

	TypeDef::GetReflectionData(outData);
	auto& map = outData.GetAsObj();

	map["hint"] = JSONValue("list");
}

void ListDef::GetKey(const TypeDef& templateDef, json_parser::JSONValue& outKey)
{
	using namespace json_parser;

	GetDefaultTypeKey(m_listDefId, outKey);
	auto& map = outKey.GetAsObj();
	map["template"] = JSONValue(templateDef.GetId());
}

const ListDef& ListDef::GetTypeDef(const TypeDef& templateDef)
{
	using namespace json_parser;

	JSONValue tmp;
	GetKey(templateDef, tmp);

	const TypeDef::TypeDefsMap& defsMap = TypeDef::GetDefsMap();

	auto it = defsMap.find(tmp.ToString(false));
	if (it != defsMap.end())
	{
		return *it->second;
	}

	ListDef* typeDef = new ListDef(templateDef);
	return *typeDef;
}

ListDef::ListDef(const TypeDef& templateDef) :
	ValueTypeDef(&GenericListDef::GetTypeDef(), m_listDefId),
	m_templateDef(templateDef)
{
	m_isGenerated = true;
}

void ListDef::GetTypeKey(json_parser::JSONValue& outTypeKey) const
{
	GetKey(m_templateDef, outTypeKey);
}

void ListDef::GetReflectionData(json_parser::JSONValue& outData)
{
	using namespace json_parser;

	TypeDef::GetReflectionData(outData);
	auto& map = outData.GetAsObj();

	map["hint"] = JSONValue("list");
}
