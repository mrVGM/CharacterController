#pragma once

#include "CompositeValue.h"
#include "CompositeTypeDef.h"

class TestObject : public ObjectValue
{
public:
	Value m_floatVal;
	Value m_stringVal;

	TestObject(const CompositeValue* outer);
};