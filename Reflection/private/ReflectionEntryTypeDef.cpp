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
	ReferenceTypeDef(&app::AppEntryTypeDef::GetAppEntryTypeDef(), "BB1063B7-C014-431C-9586-2EC9254E4892")
{
	m_name = "Reflection App Entry";
	m_category = "Reflection";
}

void reflection::ReflectionEntryTypeDef::Construct(Value& container) const
{
	ReflectionEntryObj* obj = new ReflectionEntryObj(*this, nullptr);
	container.AssignObject(obj);
}
