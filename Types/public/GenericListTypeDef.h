#pragma once

#include "TypeDef.h"

class GenericListTypeDef : public TypeDef
{
public:
	GenericListTypeDef();
	static const GenericListTypeDef& GetTypeDef();

	virtual void GetReflectionData(json_parser::JSONValue& outData) override;
};