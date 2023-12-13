#include "TestTypeDef.h"

#include "PrimitiveTypes.h"
#include "TestObject.h"


namespace
{
	TestTypeDef m_testTypeDef;

	const char* m_floatPropId = "A7BC6971-69D8-4B8D-AC13-F4F3EF513D8B";
	const char* m_stringPropId = "4C2D1D66-37DA-41F6-9B9F-A2874CE544CC";
}

TestTypeDef::TestTypeDef() :
	ReferenceTypeDef(&ReferenceTypeDef::GetTypeDef(), "B49D7893-FCA8-4CBF-8B65-D72287B94BB9"),
	m_floatProp(m_floatPropId, FloatTypeDef::GetTypeDef()),
	m_stringProp(m_stringPropId, StringTypeDef::GetTypeDef())
{
	m_name = "Test Def";

	m_floatProp.m_name = "Float Prop";
	m_floatProp.m_category = "Setup";

	m_stringProp.m_name = "String Prop";
	m_stringProp.m_category = "Setup";

	m_properties[m_floatPropId] = &m_floatProp;
	m_properties[m_stringPropId] = &m_stringProp;
}

const TestTypeDef& TestTypeDef::GetTypeDef()
{
	return m_testTypeDef;
}

void TestTypeDef::Construct(Value& container) const
{
	TestObject* obj = new TestObject(container.m_outer);
	container.m_payload = obj;
}
