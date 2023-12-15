#include "TestObject.h"

#include "PrimitiveTypes.h"
#include "TestTypeDef.h"

TestObject::TestObject(const TestTypeDef& testTypeDef, const CompositeValue* outer) :
	ObjectValue(testTypeDef, outer),
	m_floatVal(testTypeDef.m_floatProp.GetType(), this),
	m_stringVal(testTypeDef.m_stringProp.GetType(), this),
	m_listVal(testTypeDef.m_listProp.GetType(), this)
{
}
