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

void ObjectValueContainer::GetObjectsOfType(const TypeDef& typeDef, std::list<ObjectValue*>& outObjects)
{
	for (auto it = m_typedValues.begin(); it != m_typedValues.end(); ++it)
	{
		if (!TypeDef::IsA(*it->first, typeDef))
		{
			continue;
		}

		for (auto setIt = it->second.begin(); setIt != it->second.end(); ++setIt) {
			outObjects.push_back(*setIt);
		}
	}
}
