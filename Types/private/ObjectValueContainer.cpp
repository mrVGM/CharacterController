#include "ObjectValueContainer.h"

ObjectValueContainer ObjectValueContainer::m_container;

ObjectValueContainer::ObjectValueContainer()
{
}

ObjectValueContainer& ObjectValueContainer::GetContainer()
{
	return m_container;
}

void ObjectValueContainer::Register(ObjectValue* value)
{
	const TypeDef& type = value->GetTypeDef();
	m_typedValues[&type].insert(value);
}

void ObjectValueContainer::Unregister(ObjectValue* value)
{
	const TypeDef& type = value->GetTypeDef();
	m_typedValues[&type].erase(value);
}
