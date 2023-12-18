#pragma once

#include "TypeDef.h"
#include "CompositeTypeDef.h"

class GenericListDef : public TypeDef
{
	TYPE_DEF_BODY(GenericListDef)

public:
	GenericListDef();
	
	virtual void GetReflectionData(json_parser::JSONValue& outData) const override;
};
