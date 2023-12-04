#include "GenericListTypeDef.h"

#include "CompositeTypeDef.h"

namespace
{
	GenericListTypeDef m_genericListTypeDef;
}

GenericListTypeDef::GenericListTypeDef() :
	TypeDef(&ValueTypeDef::GetTypeDef(), "E454C3EC-FDE0-424F-A701-C33D4CD23E38")
{
}

const GenericListTypeDef& GenericListTypeDef::GetTypeDef()
{
	return m_genericListTypeDef;
}

void GenericListTypeDef::GetReflectionData(json_parser::JSONValue& outData)
{
	using namespace json_parser;
	TypeDef::GetReflectionData(outData);
	auto& map = outData.GetAsObj();

	map["hint"] = JSONValue("list");
}
