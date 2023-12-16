#pragma once

#include "JSONValue.h"

#include <string>
#include <map>

class Value;

template <typename T>
struct BasicObjectContainer
{
	T* m_object = nullptr;
	~BasicObjectContainer()
	{
		if (m_object)
		{
			delete m_object;
		}
		m_object = nullptr;
	}
};

class TypeDef
{
public:
	typedef std::map<std::string, const TypeDef*> TypeDefsMap;

private:
	static TypeDefsMap* m_defsMap;

	const TypeDef* m_parent = nullptr;
	std::string m_id;

protected:
	std::string m_name;
	std::string m_category;
	bool m_isGenerated = false;

	TypeDef(const TypeDef* parent, const std::string& id);

public:
	static TypeDefsMap& GetDefsMap();

	static void GetDefaultTypeKey(const std::string& id, json_parser::JSONValue& outTypeKey);

	virtual void GetTypeKey(json_parser::JSONValue& outTypeKey) const;
	virtual void GetReflectionData(json_parser::JSONValue& outData) const;

	const std::string& GetId() const;
	const TypeDef* GetParent() const;
	bool IsA(const TypeDef& type) const;

	virtual void DeserializeFromJSON(Value& value, json_parser::JSONValue& json) const;

	static void SaveReflectionData();
};