#include "CompositeTypeDef.h"

#include "CompositeValue.h"

namespace
{
	BasicObjectContainer<CompositeTypeDef> m_compositeTypeDef;
	BasicObjectContainer<ValueTypeDef> m_valueTypeDef;
	BasicObjectContainer<ReferenceTypeDef> m_referenceTypeDef;
}

CompositeTypeDef::CompositeTypeDef(const TypeDef* parent, const std::string& id, const TypeDef::TypeKeyGen& keyGenerator) :
	TypeDef(parent, id, keyGenerator)
{
	m_name = "Composite Type";
	m_category = "Composite";
}

CompositeTypeDef::CompositeTypeDef(const TypeDef* parent, const std::string& id) :
	TypeDef(parent, id)
{
	m_name = "Composite Type";
	m_category = "Composite";
}

CompositeTypeDef::~CompositeTypeDef()
{
}

const CompositeTypeDef& CompositeTypeDef::GetTypeDef()
{
	if (!m_compositeTypeDef.m_object)
	{
		m_compositeTypeDef.m_object = new CompositeTypeDef(nullptr, "1A3E4D51-E1E3-4FD4-A487-A164913A74E2");
	}
	return *m_compositeTypeDef.m_object;
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

	CompositeValue* compositeValue = value.GetValue<CompositeValue*>();
	for (auto it = m_properties.begin(); it != m_properties.end(); ++it)
	{
		Value& cur = it->second->m_getValue(compositeValue);
		cur.m_type->DeserializeFromJSON(cur, map[it->first]);
	}
}

ValueTypeDef::ValueTypeDef(const TypeDef* parent, const std::string& id, const TypeDef::TypeKeyGen& keyGenerator) :
	CompositeTypeDef(parent, id, keyGenerator)
{
	m_category = "Composite";
	m_name = "Value Type";
}

ValueTypeDef::ValueTypeDef(const TypeDef* parent, const std::string& id) :
	CompositeTypeDef(parent, id)
{
	m_category = "Composite";
	m_name = "Value Type";
}

ValueTypeDef::~ValueTypeDef()
{
}

const ValueTypeDef& ValueTypeDef::GetTypeDef()
{
	if (!m_valueTypeDef.m_object)
	{
		m_valueTypeDef.m_object = new ValueTypeDef(&CompositeTypeDef::GetTypeDef(), "43120139-ED19-4F10-8B14-703666C023AC");
	}
	return *m_valueTypeDef.m_object;
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

ReferenceTypeDef::~ReferenceTypeDef()
{
}

const ReferenceTypeDef& ReferenceTypeDef::GetTypeDef()
{
	if (!m_referenceTypeDef.m_object)
	{
		m_referenceTypeDef.m_object = new ReferenceTypeDef(&CompositeTypeDef::GetTypeDef(), "361D902F-526F-452D-B925-7B59B0F5AB09");
	}

	return *m_referenceTypeDef.m_object;
}

void ReferenceTypeDef::GetReflectionData(json_parser::JSONValue& outData) const
{
	using namespace json_parser;
	CompositeTypeDef::GetReflectionData(outData);
	auto& map = outData.GetAsObj();

	if (this == &ReferenceTypeDef::GetTypeDef())
	{
		map["hint"] = JSONValue("reference_type");
	}
}

void ReferenceTypeDef::Construct(Value& container) const
{
	throw "Can't construct from a ReferenceTypeDef!";
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
	map["type"] = m_type.GetTypeKey();

	outData = res;
}

const TypeDef& TypeProperty::GetType() const
{
	return m_type;
}

const std::string& TypeProperty::GetId() const
{
	return m_id;
}

const TypeDef& TypeProperty::GetType()
{
	return m_type;
}
