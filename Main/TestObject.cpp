#include "TestObject.h"

#include "PrimitiveTypes.h"
#include "TestTypeDef.h"

TestObject::TestObject(const CompositeValue* outer) :
	ObjectValue(TestTypeDef::GetTypeDef(), outer),
	m_floatVal(TestTypeDef::GetTypeDef().m_floatProp.GetType(), this),
	m_stringVal(TestTypeDef::GetTypeDef().m_stringProp.GetType(), this),
	m_listVal(TestTypeDef::GetTypeDef().m_listProp.GetType(), this)
{
}
