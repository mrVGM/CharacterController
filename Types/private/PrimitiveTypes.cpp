#pragma once

#include "PrimitiveTypes.h"

namespace
{
	BoolTypeDef m_boolTypeDef;
	IntTypeDef m_intTypeDef;
	FloatTypeDef m_floatTypeDef;
	StringTypeDef m_stringTypeDef;
}

const BoolTypeDef& BoolTypeDef::GetTypeDef()
{
	return m_boolTypeDef;
}

BoolTypeDef::BoolTypeDef() :
	TypeDef(nullptr, "A1A5297D-A75D-499B-8758-CC6A7696CDE0")
{
}

void BoolTypeDef::GetReflectionData(json_parser::JSONValue& outData)
{
	using namespace json_parser;

	TypeDef::GetReflectionData(outData);
	auto& map = outData.GetAsObj();
	map["hint"] = JSONValue("bool");
}

const IntTypeDef& IntTypeDef::GetTypeDef()
{
	return m_intTypeDef;
}

IntTypeDef::IntTypeDef() :
	TypeDef(nullptr, "8E993D4A-B13D-4D59-92D1-43F0FC138DBD")
{
}

void IntTypeDef::GetReflectionData(json_parser::JSONValue& outData)
{
	using namespace json_parser;

	TypeDef::GetReflectionData(outData);
	auto& map = outData.GetAsObj();
	map["hint"] = JSONValue("int");
}

const FloatTypeDef& FloatTypeDef::GetTypeDef()
{
	return m_floatTypeDef;
}

FloatTypeDef::FloatTypeDef() :
	TypeDef(nullptr, "D3B56C9E-8D0D-4065-9778-805D515D9EB6")
{
}

void FloatTypeDef::GetReflectionData(json_parser::JSONValue& outData)
{
	using namespace json_parser;

	TypeDef::GetReflectionData(outData);
	auto& map = outData.GetAsObj();
	map["hint"] = JSONValue("float");
}

const StringTypeDef& StringTypeDef::GetTypeDef()
{
	return m_stringTypeDef;
}

StringTypeDef::StringTypeDef() :
	TypeDef(nullptr, "B641BC7D-49F6-4194-B4C9-239FDDAAA623")
{
}

void StringTypeDef::GetReflectionData(json_parser::JSONValue& outData)
{
	using namespace json_parser;

	TypeDef::GetReflectionData(outData);
	auto& map = outData.GetAsObj();
	map["hint"] = JSONValue("string");
}
