#pragma once

#include "CompositeTypeDef.h"

#include "JSONValue.h"

class AssetTypeDef : public ReferenceTypeDef
{
	TYPE_DEF_BODY(AssetTypeDef)

	json_parser::JSONValue m_data;
public:
	AssetTypeDef(json_parser::JSONValue& json);
	virtual ~AssetTypeDef();

	virtual void Construct(Value& container) const override;

	virtual void DeserializeFromJSON(Value& value, json_parser::JSONValue& json) const override;
	const json_parser::JSONValue& GetJSONData() const;
};
