#pragma once

#include "CompositeValue.h"
#include "CompositeTypeDef.h"

#include "TestTypeDef.h"

class TestObject : public ObjectValue
{
public:
	Value m_floatVal;
	Value m_stringVal;
	Value m_listVal;

	TestObject(const TestTypeDef& testTypeDef, const CompositeValue* outer);
};