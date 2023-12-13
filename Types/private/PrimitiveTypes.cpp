#pragma once

#include "PrimitiveTypes.h"

#include "CompositeValue.h"

namespace
{
	std::string m_typeTypeDefId = "A4885F01-547F-4C21-BE40-A21C7BE0BA24";

	BoolTypeDef m_boolTypeDef;
	IntTypeDef m_intTypeDef;
	FloatTypeDef m_floatTypeDef;
	StringTypeDef m_stringTypeDef;
	GenericTypeDef m_genericTypedef;
}

const BoolTypeDef& BoolTypeDef::GetTypeDef()
{
	return m_boolTypeDef;
}

BoolTypeDef::BoolTypeDef() :
	TypeDef(nullptr, "A1A5297D-A75D-499B-8758-CC6A7696CDE0")
{
	m_category = "Primitive";
	m_name = "bool";
}

void BoolTypeDef::GetReflectionData(json_parser::JSONValue& outData) const
{
	using namespace json_parser;

	TypeDef::GetReflectionData(outData);
	auto& map = outData.GetAsObj();
	map["hint"] = JSONValue("bool");
}

void BoolTypeDef::DeserializeFromJSON(Value& value, json_parser::JSONValue& json) const
{
	if (!value.m_type->IsA(GetTypeDef()))
	{
		throw "Wrong value type!";
	}

	value.m_payload = static_cast<bool>(std::get<double>(json.m_payload));
}

const IntTypeDef& IntTypeDef::GetTypeDef()
{
	return m_intTypeDef;
}

IntTypeDef::IntTypeDef() :
	TypeDef(nullptr, "8E993D4A-B13D-4D59-92D1-43F0FC138DBD")
{
	m_category = "Primitive";
	m_name = "int";
}

void IntTypeDef::GetReflectionData(json_parser::JSONValue& outData) const
{
	using namespace json_parser;

	TypeDef::GetReflectionData(outData);
	auto& map = outData.GetAsObj();
	map["hint"] = JSONValue("int");
}

void IntTypeDef::DeserializeFromJSON(Value& value, json_parser::JSONValue& json) const
{
	if (!value.m_type->IsA(*this))
	{
		throw "Wrong value type!";
	}

	value.m_payload = static_cast<int>(std::get<double>(json.m_payload));
}

const FloatTypeDef& FloatTypeDef::GetTypeDef()
{
	return m_floatTypeDef;
}

FloatTypeDef::FloatTypeDef() :
	TypeDef(nullptr, "D3B56C9E-8D0D-4065-9778-805D515D9EB6")
{
	m_category = "Primitive";
	m_name = "float";
}

void FloatTypeDef::GetReflectionData(json_parser::JSONValue& outData) const
{
	using namespace json_parser;

	TypeDef::GetReflectionData(outData);
	auto& map = outData.GetAsObj();
	map["hint"] = JSONValue("float");
}

void FloatTypeDef::DeserializeFromJSON(Value& value, json_parser::JSONValue& json) const
{
	if (!value.m_type->IsA(*this))
	{
		throw "Wrong value type!";
	}

	value.m_payload = static_cast<float>(std::get<double>(json.m_payload));
}

const StringTypeDef& StringTypeDef::GetTypeDef()
{
	return m_stringTypeDef;
}

StringTypeDef::StringTypeDef() :
	TypeDef(nullptr, "B641BC7D-49F6-4194-B4C9-239FDDAAA623")
{
	m_category = "Primitive";
	m_name = "string";
}

void StringTypeDef::GetReflectionData(json_parser::JSONValue& outData) const
{
	using namespace json_parser;

	TypeDef::GetReflectionData(outData);
	auto& map = outData.GetAsObj();
	map["hint"] = JSONValue("string");
}

void StringTypeDef::DeserializeFromJSON(Value& value, json_parser::JSONValue& json) const
{
	if (!value.m_type->IsA(*this))
	{
		throw "Wrong value type!";
	}

	value.m_payload = std::get<std::string>(json.m_payload);
}

const GenericTypeDef& GenericTypeDef::GetTypeDef()
{
	return m_genericTypedef;
}

GenericTypeDef::GenericTypeDef() :
	TypeDef(nullptr, "1D4363E5-BC6F-4B49-80F3-FAC358F5B296")
{
	m_category = "Primitive";
	m_name = "type";
}

void GenericTypeDef::GetReflectionData(json_parser::JSONValue& outData) const
{
	using namespace json_parser;

	TypeDef::GetReflectionData(outData);
	auto& map = outData.GetAsObj();

	map["hint"] = JSONValue("type");
}


void TypeTypeDef::GetKey(const TypeDef& templateType, json_parser::JSONValue& outKey)
{
	using namespace json_parser;
	GetDefaultTypeKey(m_typeTypeDefId, outKey);

	auto& map = outKey.GetAsObj();
	map["template"] = JSONValue(templateType.GetId());
}

const TypeTypeDef& TypeTypeDef::GetTypeDef(const TypeDef& templateType)
{
	using namespace json_parser;

	JSONValue key;
	GetKey(templateType, key);

	TypeDefsMap& defsMap = GetDefsMap();
	auto it = defsMap.find(key.ToString(false));

	if (it != defsMap.end())
	{
		return *it->second;
	}

	TypeTypeDef* newDef = new TypeTypeDef(templateType);
	return *newDef;
}

TypeTypeDef::TypeTypeDef(const TypeDef& templateType) :
	TypeDef(&GenericTypeDef::GetTypeDef(), m_typeTypeDefId),
	m_templateType(templateType)
{
}

void TypeTypeDef::GetTypeKey(json_parser::JSONValue& outKey) const
{
	GetKey(m_templateType, outKey);
}

void TypeTypeDef::DeserializeFromJSON(Value& value, json_parser::JSONValue& json) const
{
	if (!value.m_type->IsA(*this))
	{
		throw "Wrong value type!";
	}

	using namespace json_parser;
	JSONValue key;
	GetDefaultTypeKey(m_templateType.GetId(), key);

	const TypeDef::TypeDefsMap& defsMap =TypeDef::GetDefsMap();
	auto it = defsMap.find(key.ToString(false));

	if (it == defsMap.end())
	{
		return;
	}

	value.m_payload = it->second;
}
