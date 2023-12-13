#include "CompositeTypeDef.h"

#include "CompositeValue.h"

namespace
{
	CompositeTypeDef m_compositeTypeDef(nullptr, "1A3E4D51-E1E3-4FD4-A487-A164913A74E2");
	ValueTypeDef m_valueTypeDef(&CompositeTypeDef::GetTypeDef(), "43120139-ED19-4F10-8B14-703666C023AC");
	ReferenceTypeDef m_referenceTypeDef(&CompositeTypeDef::GetTypeDef(), "361D902F-526F-452D-B925-7B59B0F5AB09");
}

CompositeTypeDef::CompositeTypeDef(const TypeDef* parent, const std::string& id) :
	TypeDef(parent, id)
{
}

const CompositeTypeDef& CompositeTypeDef::GetTypeDef()
{
	return m_compositeTypeDef;
}

void CompositeTypeDef::Construct(Value& container) const
{
	throw "Can't construct from a CompositeTypeDef!";
}

void CompositeTypeDef::GetReflectionData(json_parser::JSONValue& outData) const
{
	using namespace json_parser;

	TypeDef::GetReflectionData(outData);

	auto& map = outData.GetAsObj();
	map["properties"] = JSONValue(ValueType::Object);

	JSONValue& props = map["properties"];
	auto& propsMap = props.GetAsObj();

	for (auto it = m_properties.begin(); it != m_properties.end(); ++it)
	{
		JSONValue tmp;
		it->second->GetReflectionData(tmp);
		propsMap[it->first] = tmp;
	}
}

void CompositeTypeDef::DeserializeFromJSON(Value& value, json_parser::JSONValue& json) const
{
	auto& map = json.GetAsObj();

	CompositeValue* compositeValue = std::get<CompositeValue*>(value.m_payload);
	for (auto it = m_properties.begin(); it != m_properties.end(); ++it)
	{
		Value& cur = it->second->m_getValue(compositeValue);
		cur.m_type->DeserializeFromJSON(cur, map[it->first]);
	}
}

ValueTypeDef::ValueTypeDef(const TypeDef* parent, const std::string& id) :
	CompositeTypeDef(parent, id)
{
	m_category = "Composite";
	m_name = "Value Type";
}

const ValueTypeDef& ValueTypeDef::GetTypeDef()
{
	return m_valueTypeDef;
}

void ValueTypeDef::GetReflectionData(json_parser::JSONValue& outData) const
{
	using namespace json_parser;
	CompositeTypeDef::GetReflectionData(outData);
	auto& map = outData.GetAsObj();

	map["hint"] = JSONValue("value_type");
}

ReferenceTypeDef::ReferenceTypeDef(const TypeDef* parent, const std::string& id) :
	CompositeTypeDef(parent, id)
{
	m_category = "Composite";
	m_name = "Reference Type";
}

const ReferenceTypeDef& ReferenceTypeDef::GetTypeDef()
{
	return m_referenceTypeDef;
}

void ReferenceTypeDef::GetReflectionData(json_parser::JSONValue& outData) const
{
	using namespace json_parser;
	CompositeTypeDef::GetReflectionData(outData);
	auto& map = outData.GetAsObj();

	map["hint"] = JSONValue("reference_type");
}

void ReferenceTypeDef::Construct(Value& container) const
{
	throw "Can't construct from a ReferenceTypeDef!";
}

void ReferenceTypeDef::DeserializeFromJSON(Value& value, json_parser::JSONValue& json) const
{
}

TypeProperty::TypeProperty(const std::string id, const TypeDef& type) :
	m_id(id),
	m_type(type)
{
}

void TypeProperty::GetReflectionData(json_parser::JSONValue& outData) const
{
	using namespace json_parser;
	JSONValue res(ValueType::Object);

	auto& map = res.GetAsObj();
	map["id"] = JSONValue(m_id);
	map["name"] = JSONValue(m_name);
	map["category"] = JSONValue(m_category);

	JSONValue type;
	m_type.GetTypeKey(type);
	map["type"] = type;

	outData = res;
}

const std::string& TypeProperty::GetId()
{
	return m_id;
}

const TypeDef& TypeProperty::GetType()
{
	return m_type;
}
