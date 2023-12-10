#pragma once

#include "TypeDef.h"

class ObjectValue;
class Value;

class CompositeTypeDef : public TypeDef
{
public:
	CompositeTypeDef(const TypeDef* parent, const std::string& id);
	static const CompositeTypeDef& GetTypeDef();

	virtual void Construct(Value& container) const;
};

class ValueTypeDef : public CompositeTypeDef
{
public:
	ValueTypeDef(const TypeDef* parent, const std::string& id);
	static const ValueTypeDef& GetTypeDef();

	virtual void GetReflectionData(json_parser::JSONValue& outData) override;
};

class ReferenceTypeDef : public CompositeTypeDef
{
public:
	ReferenceTypeDef(const TypeDef* parent, const std::string& id);
	static const ReferenceTypeDef& GetTypeDef();

	virtual void GetReflectionData(json_parser::JSONValue& outData) override;
	virtual void Construct(Value& container) const override;
};