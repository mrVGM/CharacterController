#pragma once

#include "GenericListDef.h"

class ListDef : public ValueTypeDef
{
	TYPE_DEF_BODY(ListDef)

private:
	static void GetKey(const TypeDef& templateDef, json_parser::JSONValue& outKey);

public:
	static const ListDef& GetTypeDef(const TypeDef& templateDef);
	const TypeDef& m_templateDef;

	explicit ListDef(const TypeDef& templateDef);

	virtual void Construct(Value& container) const override;

	virtual void GetTypeKey(json_parser::JSONValue& outTypeKey) const;
	virtual void GetReflectionData(json_parser::JSONValue& outData) const override;

	virtual void DeserializeFromJSON(Value& value, json_parser::JSONValue& json) const;
};