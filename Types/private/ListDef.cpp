#include "ListDef.h"

#include "GenericListDef.h"
#include "ValueList.h"

namespace
{
	const char* m_listDefId = "61BE566E-F055-4537-BCBD-2E1A7335EB55";

	struct ListKeyGen : public TypeDef::TypeKeyGen
	{
	private:
		const TypeDef& m_template;

	public:
		ListKeyGen(const TypeDef& templateType) :
			m_template(templateType)
		{
		}

		void GenerateKey(json_parser::JSONValue& key) const override
		{
			using namespace json_parser;
			JSONValue tmp;
			TypeDef::GetDefaultTypeKey(GenericListDef::GetTypeDef().GetId(), tmp);

			auto& map = tmp.GetAsObj();
			map["template"] = JSONValue(m_template.GetTypeKey());

			key = tmp;
		}
	};
}

const ListDef& ListDef::GetTypeDef(const TypeDef& templateDef)
{
	using namespace json_parser;

	JSONValue tmp;
	{
		ListKeyGen keyGen(templateDef);
		keyGen.GenerateKey(tmp);
	}

	TypeDef::TypeDefsMap& defsMap = TypeDef::GetDefsMap();
	const TypeDef* type = defsMap.GetByKey(tmp.ToString(false));
	if (type)
	{
		return static_cast<const ListDef&>(*type);
	}

	ListDef* typeDef = new ListDef(templateDef);
	return *typeDef;
}

ListDef::ListDef(const TypeDef& templateDef) :
	ValueTypeDef(&GenericListDef::GetTypeDef(), m_listDefId, ListKeyGen(templateDef)),
	m_templateDef(templateDef)
{
	m_isGenerated = true;
}

ListDef::~ListDef()
{
}

void ListDef::Construct(Value& container) const
{
	ValueList* tmp = new ValueList(*this, container.m_outer);
	container.m_payload = tmp;
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

	if (!TypeDef::IsA(*value.m_type, *this))
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
