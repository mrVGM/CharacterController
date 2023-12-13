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
protected:
	std::map<std::string, const TypeProperty*> m_properties;
public:
	CompositeTypeDef(const TypeDef* parent, const std::string& id);
	static const CompositeTypeDef& GetTypeDef();

	virtual void Construct(Value& container) const;
	virtual void GetReflectionData(json_parser::JSONValue& outData) const override;
	virtual void DeserializeFromJSON(Value& value, json_parser::JSONValue& json) const;
};

class ValueTypeDef : public CompositeTypeDef
{
public:
	ValueTypeDef(const TypeDef* parent, const std::string& id);
	static const ValueTypeDef& GetTypeDef();

	virtual void GetReflectionData(json_parser::JSONValue& outData) const override;
};

class ReferenceTypeDef : public CompositeTypeDef
{
public:
	ReferenceTypeDef(const TypeDef* parent, const std::string& id);
	static const ReferenceTypeDef& GetTypeDef();

	virtual void GetReflectionData(json_parser::JSONValue& outData) const override;
	virtual void Construct(Value& container) const override;
	virtual void DeserializeFromJSON(Value& value, json_parser::JSONValue& json) const;
};