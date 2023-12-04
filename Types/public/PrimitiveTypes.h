#pragma once

#include "TypeDef.h"

class BoolTypeDef : public TypeDef
{
public:
	static const BoolTypeDef& GetTypeDef();

	BoolTypeDef();

	virtual void GetReflectionData(json_parser::JSONValue& outData) override;
};

class IntTypeDef : public TypeDef
{
public:
	static const IntTypeDef& GetTypeDef();

	IntTypeDef();

	virtual void GetReflectionData(json_parser::JSONValue& outData) override;
};

class FloatTypeDef : public TypeDef
{
public:
	static const FloatTypeDef& GetTypeDef();

	FloatTypeDef();

	virtual void GetReflectionData(json_parser::JSONValue& outData) override;
};

class StringTypeDef : public TypeDef
{
public:
	static const StringTypeDef& GetTypeDef();

	StringTypeDef();

	virtual void GetReflectionData(json_parser::JSONValue& outData) override;
};
