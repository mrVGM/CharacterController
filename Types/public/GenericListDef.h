#pragma once

#include "TypeDef.h"
#include "CompositeTypeDef.h"

class GenericListDef : public TypeDef
{
public:
	static const GenericListDef& GetTypeDef();

	GenericListDef();
	
	virtual void GetReflectionData(json_parser::JSONValue& outData) const override;
};
