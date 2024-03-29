#pragma once

#include "TypeDef.h"

class BoolTypeDef : public TypeDef
{
	TYPE_DEF_BODY(BoolTypeDef)

public:
	BoolTypeDef();
	virtual ~BoolTypeDef();

	virtual void GetReflectionData(json_parser::JSONValue& outData) const override;
	virtual void DeserializeFromJSON(Value& value, json_parser::JSONValue& json) const override;
};
 
class IntTypeDef : public TypeDef
{
	TYPE_DEF_BODY(IntTypeDef)

public:
	IntTypeDef();
	virtual ~IntTypeDef();

	virtual void GetReflectionData(json_parser::JSONValue& outData) const override;
	virtual void DeserializeFromJSON(Value& value, json_parser::JSONValue& json) const override;
};

class FloatTypeDef : public TypeDef
{
	TYPE_DEF_BODY(FloatTypeDef)

public:
	FloatTypeDef();
	virtual ~FloatTypeDef();

	virtual void GetReflectionData(json_parser::JSONValue& outData) const override;
	virtual void DeserializeFromJSON(Value& value, json_parser::JSONValue& json) const;
};

class StringTypeDef : public TypeDef
{
	TYPE_DEF_BODY(StringTypeDef)

public:
	StringTypeDef();
	virtual ~StringTypeDef();

	virtual void GetReflectionData(json_parser::JSONValue& outData) const override;
	virtual void DeserializeFromJSON(Value& value, json_parser::JSONValue& json) const;
};

class GenericTypeDef : public TypeDef
{
	TYPE_DEF_BODY(GenericTypeDef)

public:
	GenericTypeDef();
	virtual ~GenericTypeDef();

	virtual void GetReflectionData(json_parser::JSONValue& outData) const override;
};

class TypeTypeDef : public TypeDef
{
	TYPE_DEF_BODY(TypeTypeDef)

private:
	const TypeDef& m_templateType;

public:
	static const TypeTypeDef& GetTypeDef(const TypeDef& templateType);
	virtual ~TypeTypeDef();

	explicit TypeTypeDef(const TypeDef& templateType);
	virtual void DeserializeFromJSON(Value& value, json_parser::JSONValue& json) const;
};

