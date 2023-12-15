#include "AssetTypeDef.h"

namespace
{
	const ReferenceTypeDef* GetParentFromJSON(json_parser::JSONValue& json)
	{
		using namespace json_parser;

		auto& map = json.GetAsObj();
		JSONValue parent = map["parent"];

		TypeDef::TypeDefsMap& defsMap = TypeDef::GetDefsMap();
		auto it = defsMap.find(parent.ToString(false));

		if (it == defsMap.end())
		{
			throw "Can't find parent type def!";
		}

		const TypeDef* type = it->second;
		if (!type->IsA(ReferenceTypeDef::GetTypeDef()))
		{
			throw "Can't cast to reference def!";
		}

		return static_cast<const ReferenceTypeDef*>(type);
	}

	std::string GetIdFromJSON(json_parser::JSONValue& json)
	{
		using namespace json_parser;

		auto& map = json.GetAsObj();
		JSONValue id = map["id"];

		return id.ToString(false);
	}
}

AssetTypeDef::AssetTypeDef(json_parser::JSONValue& json) :
	ReferenceTypeDef(GetParentFromJSON(json), GetIdFromJSON(json))
{
	using namespace json_parser;

	auto& map = json.GetAsObj();
	JSONValue name = map["name"];
	JSONValue category = map["category"];

	m_name = std::get<std::string>(name.m_payload);
	m_category = std::get<std::string>(category.m_payload);
}

void AssetTypeDef::Construct(Value& container) const
{
	const ReferenceTypeDef* parent = static_cast<const ReferenceTypeDef*>(GetParent());
	parent->Construct(container);
}