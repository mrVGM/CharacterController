#include "CompositeTypeDef.h"

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


ValueTypeDef::ValueTypeDef(const TypeDef* parent, const std::string& id) :
	CompositeTypeDef(parent, id)
{
}

const ValueTypeDef& ValueTypeDef::GetTypeDef()
{
	return m_valueTypeDef;
}

void ValueTypeDef::GetReflectionData(json_parser::JSONValue& outData)
{
	using namespace json_parser;
	CompositeTypeDef::GetReflectionData(outData);
	auto& map = outData.GetAsObj();

	map["hint"] = JSONValue("value_type");
}

ReferenceTypeDef::ReferenceTypeDef(const TypeDef* parent, const std::string& id) :
	CompositeTypeDef(parent, id)
{
}

const ReferenceTypeDef& ReferenceTypeDef::GetTypeDef()
{
	return m_referenceTypeDef;
}

void ReferenceTypeDef::GetReflectionData(json_parser::JSONValue& outData)
{
	using namespace json_parser;
	CompositeTypeDef::GetReflectionData(outData);
	auto& map = outData.GetAsObj();

	map["hint"] = JSONValue("reference_type");
}

void ReferenceTypeDef::Contruct(ObjectValue& outValue)
{
}
