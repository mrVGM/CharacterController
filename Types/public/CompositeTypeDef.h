#pragma once

#include "TypeDef.h"

#include <functional>

class ObjectValue;
class Value;

class CompositeValue;

class TypeProperty
{
private:
	std::string m_id;
	const TypeDef& m_type;

public:
	std::string m_name;
	std::string m_category;

	const std::string& GetId();
	const TypeDef& GetType();

	std::function<Value&(CompositeValue*)> m_getValue;

	TypeProperty(const std::string id, const TypeDef& type);
	void GetReflectionData(json_parser::JSONValue& outData) const;
	const TypeDef& GetType() const;
};

class CompositeTypeDef : public TypeDef
{
	TYPE_DEF_BODY(CompositeTypeDef)

protected:
	std::map<std::string, const TypeProperty*> m_properties;
	CompositeTypeDef(const TypeDef* parent, const std::string& id, const TypeDef::TypeKeyGen& keyGenerator);

public:
	CompositeTypeDef(const TypeDef* parent, const std::string& id);

	virtual void Construct(Value& container) const;
	virtual void GetReflectionData(json_parser::JSONValue& outData) const override;
	virtual void DeserializeFromJSON(Value& value, json_parser::JSONValue& json) const;
};

class ValueTypeDef : public CompositeTypeDef
{
	TYPE_DEF_BODY(ValueTypeDef)

protected:
	ValueTypeDef(const TypeDef* parent, const std::string& id, const TypeDef::TypeKeyGen& keyGenerator);

public:
	ValueTypeDef(const TypeDef* parent, const std::string& id);

	virtual void GetReflectionData(json_parser::JSONValue& outData) const override;
};

class ReferenceTypeDef : public CompositeTypeDef
{
	TYPE_DEF_BODY(ReferenceTypeDef)

public:
	ReferenceTypeDef(const TypeDef* parent, const std::string& id);
	
	virtual void GetReflectionData(json_parser::JSONValue& outData) const override;
	virtual void Construct(Value& container) const override;
};