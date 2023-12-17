#pragma once

#include "CompositeTypeDef.h"

#include "JSONValue.h"

class AssetTypeDef : public ReferenceTypeDef
{
	json_parser::JSONValue m_data;
public:
	AssetTypeDef(json_parser::JSONValue& json);
	virtual void Construct(Value& container) const override;
};
