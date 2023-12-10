#pragma once

#include "TypeDef.h"
#include "CompositeTypeDef.h"

class GenericListDef : public TypeDef
{
public:
	static const GenericListDef& GetTypeDef();

	GenericListDef();
	
	virtual void GetReflectionData(json_parser::JSONValue& outData) override;
};


class ListDef : public ValueTypeDef
{
private:
	static void GetKey(const TypeDef& templateDef, json_parser::JSONValue& outKey);

public:
	static const ListDef& GetTypeDef(const TypeDef& templateDef);
	const TypeDef& m_templateDef;

	ListDef(const TypeDef& templateDef);

	virtual void Construct(Value& container) const override;

	virtual void GetTypeKey(json_parser::JSONValue& outTypeKey) const;
	virtual void GetReflectionData(json_parser::JSONValue& outData) override;
};