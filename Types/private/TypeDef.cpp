#include "TypeDef.h"

TypeDef::TypeDefsMap* TypeDef::m_defsMap = nullptr;

TypeDef::TypeDef(const TypeDef* parent, const std::string& id) :
	m_id(id),
	m_parent(parent)
{
	using namespace json_parser;
	JSONValue tmp;
	GetTypeKey(tmp);

	GetDefsMap()[tmp.ToString(false)] = this;
}

void TypeDef::GetTypeKey(json_parser::JSONValue& outTypeKey) const
{
	GetDefaultTypeKey(m_id, outTypeKey);
}

TypeDef::TypeDefsMap& TypeDef::GetDefsMap()
{
	if (!m_defsMap)
	{
		m_defsMap = new TypeDefsMap();
	}
	return *m_defsMap;
}

void TypeDef::GetDefaultTypeKey(const std::string& id, json_parser::JSONValue& outTypeKey)
{
	using namespace json_parser;
	json_parser::JSONValue res(ValueType::Object);
	auto& map = res.GetAsObj();

	map["id"] = JSONValue(id);

	outTypeKey = res;
}

void TypeDef::GetReflectionData(json_parser::JSONValue& outData)
{
	using namespace json_parser;

	outData = json_parser::JSONValue(ValueType::Object);
	auto& map = outData.GetAsObj();

	JSONValue typeKey;
	GetTypeKey(typeKey);
	map["id"] = typeKey;

	if (m_parent)
	{
		JSONValue parentTypeKey;
		m_parent->GetTypeKey(parentTypeKey);
		map["parent"] = typeKey;
	}

	map["name"] = JSONValue(m_name);
	map["category"] = JSONValue(m_category);
	map["isGenerated"] = JSONValue(static_cast<double>(m_isGenerated));
}

const std::string& TypeDef::GetId() const
{
	return m_id;
}

const TypeDef* TypeDef::GetParent() const
{
	return m_parent;
}
