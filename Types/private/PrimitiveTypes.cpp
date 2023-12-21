#pragma once

#include "PrimitiveTypes.h"

#include "CompositeValue.h"

namespace
{
	const char* m_typeTypeDefId = "A4885F01-547F-4C21-BE40-A21C7BE0BA24";
	const char* m_genericTypeDefId = "1D4363E5-BC6F-4B49-80F3-FAC358F5B296";

	BasicObjectContainer<BoolTypeDef> m_boolTypeDef;
	BasicObjectContainer<IntTypeDef> m_intTypeDef;
	BasicObjectContainer<FloatTypeDef> m_floatTypeDef;
	BasicObjectContainer<StringTypeDef> m_stringTypeDef;
	BasicObjectContainer<GenericTypeDef> m_genericTypedef;

	struct TypeTypeKeyGen : public TypeDef::TypeKeyGen
	{
	private:
		const TypeDef& m_template;

	public:
		TypeTypeKeyGen(const TypeDef& templateType) :
			m_template(templateType)
		{
		}

		void GenerateKey(json_parser::JSONValue& key) const override
		{
			using namespace json_parser;
			JSONValue tmp;
			TypeDef::GetDefaultTypeKey(m_genericTypeDefId, tmp);

			auto& map = tmp.GetAsObj();
			map["template"] = JSONValue(m_template.GetId());

			key = tmp;
		}
	};
}

const BoolTypeDef& BoolTypeDef::GetTypeDef()
{
	if (!m_boolTypeDef.m_object)
	{
		m_boolTypeDef.m_object = new BoolTypeDef();
	}

	return *m_boolTypeDef.m_object;
}

BoolTypeDef::BoolTypeDef() :
	TypeDef(nullptr, "A1A5297D-A75D-499B-8758-CC6A7696CDE0")
{
	m_category = "Primitive";
	m_name = "bool";
}

BoolTypeDef::~BoolTypeDef()
{
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
	if (!IsA(*value.m_type, *this))
	{
		throw "Wrong value type!";
	}

	value.m_payload = static_cast<bool>(std::get<double>(json.m_payload));
}

const IntTypeDef& IntTypeDef::GetTypeDef()
{
	if (!m_intTypeDef.m_object)
	{
		m_intTypeDef.m_object = new IntTypeDef();
	}
	return *m_intTypeDef.m_object;
}

IntTypeDef::IntTypeDef() :
	TypeDef(nullptr, "8E993D4A-B13D-4D59-92D1-43F0FC138DBD")
{
	m_category = "Primitive";
	m_name = "int";
}

IntTypeDef::~IntTypeDef()
{
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
	if (!TypeDef::IsA(*value.m_type, *this))
	{
		throw "Wrong value type!";
	}

	value.m_payload = static_cast<int>(std::get<double>(json.m_payload));
}

const FloatTypeDef& FloatTypeDef::GetTypeDef()
{
	if (!m_floatTypeDef.m_object)
	{
		m_floatTypeDef.m_object = new FloatTypeDef();
	}
	return *m_floatTypeDef.m_object;
}

FloatTypeDef::FloatTypeDef() :
	TypeDef(nullptr, "D3B56C9E-8D0D-4065-9778-805D515D9EB6")
{
	m_category = "Primitive";
	m_name = "float";
}

FloatTypeDef::~FloatTypeDef()
{
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
	if (!TypeDef::IsA(*value.m_type, *this))
	{
		throw "Wrong value type!";
	}

	value.m_payload = static_cast<float>(std::get<double>(json.m_payload));
}

const StringTypeDef& StringTypeDef::GetTypeDef()
{
	if (!m_stringTypeDef.m_object)
	{
		m_stringTypeDef.m_object = new StringTypeDef();
	}
	return *m_stringTypeDef.m_object;
}

StringTypeDef::StringTypeDef() :
	TypeDef(nullptr, "B641BC7D-49F6-4194-B4C9-239FDDAAA623")
{
	m_category = "Primitive";
	m_name = "string";
}

StringTypeDef::~StringTypeDef()
{
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
	if (!TypeDef::IsA(*value.m_type, *this))
	{
		throw "Wrong value type!";
	}

	value.m_payload = std::get<std::string>(json.m_payload);
}

const GenericTypeDef& GenericTypeDef::GetTypeDef()
{
	if (!m_genericTypedef.m_object)
	{
		m_genericTypedef.m_object = new GenericTypeDef();
	}

	return *m_genericTypedef.m_object;
}

GenericTypeDef::GenericTypeDef() :
	TypeDef(nullptr, m_genericTypeDefId)
{
	m_category = "Primitive";
	m_name = "type";
}

GenericTypeDef::~GenericTypeDef()
{
}

void GenericTypeDef::GetReflectionData(json_parser::JSONValue& outData) const
{
	using namespace json_parser;

	TypeDef::GetReflectionData(outData);
	auto& map = outData.GetAsObj();

	map["hint"] = JSONValue("type");
}

const TypeTypeDef& TypeTypeDef::GetTypeDef(const TypeDef& templateType)
{
	using namespace json_parser;

	JSONValue key;
	{
		TypeTypeKeyGen tmp(templateType);
		tmp.GenerateKey(key);
	}

	TypeDefsMap& defsMap = GetDefsMap();
	auto it = defsMap.find(key.ToString(false));

	if (it != defsMap.end())
	{
		return static_cast<TypeTypeDef>(*it->second);
	}

	TypeTypeDef* newDef = new TypeTypeDef(templateType);
	return *newDef;
}

TypeTypeDef::TypeTypeDef(const TypeDef& templateType) :
	TypeDef(&GenericTypeDef::GetTypeDef(), m_typeTypeDefId, TypeTypeKeyGen(templateType)),
	m_templateType(templateType)
{
	m_isGenerated = true;
}

TypeTypeDef::~TypeTypeDef()
{
}

void TypeTypeDef::DeserializeFromJSON(Value& value, json_parser::JSONValue& json) const
{
	if (!TypeDef::IsA(*value.m_type, *this))
	{
		throw "Wrong value type!";
	}

	using namespace json_parser;

	const TypeDef::TypeDefsMap& defsMap = TypeDef::GetDefsMap();
	auto it = defsMap.find(json.ToString(false));

	if (it == defsMap.end())
	{
		return;
	}

	value.m_payload = it->second;
}
