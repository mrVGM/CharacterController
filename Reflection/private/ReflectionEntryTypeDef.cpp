#include "ReflectionEntryTypeDef.h"

#include "ReflectionEntryObj.h"

namespace
{
	BasicObjectContainer<reflection::ReflectionEntryTypeDef> m_reflectionEntryTypeDef;
}

const reflection::ReflectionEntryTypeDef& reflection::ReflectionEntryTypeDef::GetReflectionEntryTypeDef()
{
	if (!m_reflectionEntryTypeDef.m_object)
	{
		m_reflectionEntryTypeDef.m_object = new ReflectionEntryTypeDef();
	}

	return *m_reflectionEntryTypeDef.m_object;
}

reflection::ReflectionEntryTypeDef::ReflectionEntryTypeDef() :
	ReferenceTypeDef(&ReferenceTypeDef::GetReferenceTypeDef(), "D3C7E002-FB82-486C-AF48-A49064A16502")
{
	m_name = "Reflection App Entry";
	m_category = "Reflection";
}

void reflection::ReflectionEntryTypeDef::Construct(Value& container) const
{
	ReflectionEntryObj* obj = new ReflectionEntryObj(*this, nullptr);
	container.AssignObject(obj);
}
