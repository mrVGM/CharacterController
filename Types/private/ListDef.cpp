#include "ListDef.h"

#include "GenericListDef.h"
#include "ValueList.h"

namespace
{
	const char* m_listDefId = "61BE566E-F055-4537-BCBD-2E1A7335EB55";
}

void ListDef::GetKey(const TypeDef& templateDef, json_parser::JSONValue& outKey)
{
	using namespace json_parser;

	const GenericListDef& genericListDef = GenericListDef::GetTypeDef();
	JSONValue genericListDefId;
	genericListDef.GetTypeKey(genericListDefId);

	std::string id;
	{
		const auto& map = genericListDefId.GetAsObj();
		JSONValue tmp = map.find("id")->second;
		id = std::get<std::string>(tmp.m_payload);
	}

	GetDefaultTypeKey(id, outKey);
	auto& map = outKey.GetAsObj();
	map["template"] = JSONValue(templateDef.GetId());
}

const ListDef& ListDef::GetTypeDef(const TypeDef& templateDef)
{
	using namespace json_parser;

	JSONValue tmp;
	GetKey(templateDef, tmp);

	const TypeDef::TypeDefsMap& defsMap = TypeDef::GetDefsMap();

	auto it = defsMap.find(tmp.ToString(false));
	if (it != defsMap.end())
	{
		return static_cast<const ListDef&>(*it->second);
	}

	ListDef* typeDef = new ListDef(templateDef);
	return *typeDef;
}

ListDef::ListDef(const TypeDef& templateDef) :
	ValueTypeDef(&GenericListDef::GetTypeDef(), m_listDefId),
	m_templateDef(templateDef)
{
	m_isGenerated = true;
}

void ListDef::Construct(Value& container) const
{
	ValueList* tmp = new ValueList(*this, container.m_outer);
	container.m_payload = tmp;
}

void ListDef::GetTypeKey(json_parser::JSONValue& outTypeKey) const
{
	GetKey(m_templateDef, outTypeKey);
}

void ListDef::GetReflectionData(json_parser::JSONValue& outData) const
{
	using namespace json_parser;

	TypeDef::GetReflectionData(outData);
	auto& map = outData.GetAsObj();

	map["hint"] = JSONValue("list");
}

void ListDef::DeserializeFromJSON(Value& value, json_parser::JSONValue& json) const
{
	auto list = json.GetAsList();

	if (!value.m_type->IsA(*this))
	{
		throw "Wrong value type!";
	}

	CompositeValue* compositeValue = value.GetValue<CompositeValue*>();
	ValueList* valueList = static_cast<ValueList*>(compositeValue);

	valueList->Clear();
	for (auto it = list.begin(); it != list.end(); ++it)
	{
		Value& cur = valueList->EmplaceBack();
		m_templateDef.DeserializeFromJSON(cur, *it);
	}
}
