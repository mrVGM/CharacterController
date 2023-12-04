#pragma once

#include "CompositeTypeDef.h"

class ListTypeDef : public ValueTypeDef
{
private:
	const TypeDef& m_templateType;

	static void GetKey(const TypeDef& templateType, json_parser::JSONValue& outTypeKey);

protected:
	virtual void GetTypeKey(json_parser::JSONValue& outTypeKey) const override;

public:
	static const ListTypeDef& GetTypeDef(const TypeDef& templateType);

	ListTypeDef(const TypeDef& templateType);
};