#include "AssetTypeDef.h"
#include "Value.h"
#include "CompositeValue.h"
#include "ObjectValueContainer.h"

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
		if (!type->IsA(ReferenceTypeDef::GetReferenceTypeDef()))
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

		auto& idMap = id.GetAsObj();

		return std::get<std::string>(idMap["id"].m_payload);
	}
}

AssetTypeDef::AssetTypeDef(json_parser::JSONValue& json) :
	ReferenceTypeDef(GetParentFromJSON(json), GetIdFromJSON(json)),
	m_data(json)
{
	using namespace json_parser;

	m_isGenerated = true;

	auto& map = json.GetAsObj();
	JSONValue name = map["name"];
	JSONValue category = map["category"];

	m_name = std::get<std::string>(name.m_payload);
	m_category = std::get<std::string>(category.m_payload);
}

void AssetTypeDef::Construct(Value& container) const
{
	ObjectValueContainer& objectValueContainer = ObjectValueContainer::GetContainer();

	const ReferenceTypeDef* parent = static_cast<const ReferenceTypeDef*>(GetParent());
	parent->Construct(container);

	ObjectValue* tmp = container.GetValue<ObjectValue*>();
	objectValueContainer.Unregister(tmp);

	tmp->m_typeDef = this;
	objectValueContainer.Register(tmp);
}

void AssetTypeDef::DeserializeFromJSON(Value& value, json_parser::JSONValue& json) const
{
	const ReferenceTypeDef* parent = static_cast<const ReferenceTypeDef*>(GetParent());
	parent->DeserializeFromJSON(value, json);
}
