#pragma once

#include "CompositeTypeDef.h"

#include "JSONValue.h"

class AssetTypeDef : public ReferenceTypeDef
{
public:
	AssetTypeDef(json_parser::JSONValue& json);
	virtual void Construct(Value& container) const override;
};
