#pragma once

#include "JSONValue.h"

#include <string>
#include <map>
#include <mutex>
#include <functional>

#define TYPE_DEF_BODY(Name)\
public:\
	static const Name& GetTypeDef();\


class Value;

template <typename T>
struct BasicObjectContainer
{
	T* m_object = nullptr;
	void Dispose()
	{
		if (m_object)
		{
			delete m_object;
		}
		m_object = nullptr;
	}
	~BasicObjectContainer()
	{
		Dispose();
	}
};

class TypeDef
{
	TYPE_DEF_BODY(TypeDef)

public:

	struct TypeDefsMap
	{
	private:
		std::mutex m_mutex;
		std::map<std::string, const TypeDef*> m_map;
	public:
		const TypeDef* GetByKey(const std::string& key);
		const TypeDef* GetByFilter(const std::function<bool(const TypeDef*)>& filter);
		void Register(const std::string& key, const TypeDef* type);
		void Iterate(const std::function<void(const TypeDef*)>& func);
	};

	struct TypeKeyGen
	{
		virtual void GenerateKey(json_parser::JSONValue& key) const = 0;
	};

private:

	const TypeDef* m_parent = nullptr;
	std::string m_id;
	json_parser::JSONValue m_typeKey;

protected:
	std::string m_name;
	std::string m_category;
	bool m_isGenerated = false;

	TypeDef(const TypeDef* parent, const std::string& id, const TypeKeyGen& keyGenerator);
	TypeDef(const TypeDef* parent, const std::string& id);

public:
	virtual ~TypeDef();

	static TypeDefsMap& GetDefsMap();

	static void GetDefaultTypeKey(const std::string& id, json_parser::JSONValue& outTypeKey);

	const json_parser::JSONValue& GetTypeKey() const;
	virtual void GetReflectionData(json_parser::JSONValue& outData) const;

	const std::string& GetId() const;
	const TypeDef* GetParent() const;
	static bool IsA(const TypeDef& type1, const TypeDef& type2);

	virtual void DeserializeFromJSON(Value& value, json_parser::JSONValue& json) const;

	static void SaveReflectionData();

	bool IsGenerated() const;
};