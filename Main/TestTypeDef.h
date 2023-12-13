#pragma once

#include "CompositeTypeDef.h"
#include "CompositeValue.h"


class TestTypeDef : public ReferenceTypeDef
{
public:
	TypeProperty m_floatProp;
	TypeProperty m_stringProp;

	TestTypeDef();
	static const TestTypeDef& GetTypeDef();

	virtual void Construct(Value& container) const override;
};