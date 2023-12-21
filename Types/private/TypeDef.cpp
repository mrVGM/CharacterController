#include "TypeDef.h"
#include "Files.h"

namespace
{
	BasicObjectContainer<TypeDef::TypeDefsMap> m_typeDefsMap;

	class KeyGen : public TypeDef::TypeKeyGen
	{
	private:
		std::string m_id;
	public:
		KeyGen(const std::string& id) :
			m_id(id)
		{
		}
		void GenerateKey(json_parser::JSONValue& key) const override
		{
			using namespace json_parser;

			JSONValue tmp;
			TypeDef::GetDefaultTypeKey(m_id, tmp);
			key = tmp;
		}
	};
}

TypeDef::TypeDef(const TypeDef* parent, const std::string& id, const TypeDef::TypeKeyGen& keyGenerator) :
	m_id(id),
	m_parent(parent)
{
	keyGenerator.GenerateKey(m_typeKey);
	TypeDefsMap& defsMap = GetDefsMap();

	std::string typeKey = m_typeKey.ToString(false);
	defsMap[typeKey] = this;
}

TypeDef::TypeDef(const TypeDef* parent, const std::string& id) :
	TypeDef(parent, id, KeyGen(id))
{
}

TypeDef::~TypeDef()
{
}

const json_parser::JSONValue& TypeDef::GetTypeKey() const
{
	return m_typeKey;
}

TypeDef::TypeDefsMap& TypeDef::GetDefsMap()
{
	if (!m_typeDefsMap.m_object)
	{
		m_typeDefsMap.m_object = new TypeDefsMap();
	}
	return *m_typeDefsMap.m_object;
}

bool TypeDef::IsA(const TypeDef& type1, const TypeDef& type2)
{
	const TypeDef* cur = &type1;

	while (cur)
	{
		if (cur == &type2)
		{
			return true;
		}
		cur = cur->GetParent();
	}

	return false;
}

void TypeDef::DeserializeFromJSON(Value& value, json_parser::JSONValue& json) const
{
}

void TypeDef::SaveReflectionData()
{
	using namespace json_parser;
	TypeDefsMap& defsMap = GetDefsMap();
	for (auto it = defsMap.begin(); it != defsMap.end(); ++it)
	{
		if (it->second->m_isGenerated)
		{
			continue;
		}
		
		JSONValue reflectionData;
		it->second->GetReflectionData(reflectionData);

		std::string contents = reflectionData.ToString(true);
		std::string fileName = files::GetReflectionDataDir() + it->second->GetId() + ".json";

		files::WriteTextFile(fileName, contents);
	}
}

void TypeDef::GetDefaultTypeKey(const std::string& id, json_parser::JSONValue& outTypeKey)
{
	using namespace json_parser;
	json_parser::JSONValue res(ValueType::Object);
	auto& map = res.GetAsObj();

	map["id"] = JSONValue(id);

	outTypeKey = res;
}

void TypeDef::GetReflectionData(json_parser::JSONValue& outData) const
{
	using namespace json_parser;

	outData = json_parser::JSONValue(ValueType::Object);
	auto& map = outData.GetAsObj();

	JSONValue typeKey = m_typeKey;
	map["id"] = typeKey;

	if (m_parent)
	{
		map["parent"] = m_parent->m_typeKey;
	}

	map["name"] = JSONValue(m_name);
	map["category"] = JSONValue(m_category);
	map["isGenerated"] = JSONValue(static_cast<double>(m_isGenerated));
}

const std::string& TypeDef::GetId() const
{
	return m_id;
}

const TypeDef* TypeDef::GetParent() const
{
	return m_parent;
}
