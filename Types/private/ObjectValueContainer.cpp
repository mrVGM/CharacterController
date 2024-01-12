#include "ObjectValueContainer.h"

ObjectValueContainer ObjectValueContainer::m_container;

ObjectValueContainer::ObjectValueContainer()
{
}

void ObjectValueContainer::CheckAccess()
{
	if (!m_exclusiveThreadAccess)
	{
		return;
	}

	if (m_exclusiveThreadAccessId != std::this_thread::get_id())
	{
		throw "Illegal Access!";
	}
}

ObjectValueContainer& ObjectValueContainer::GetContainer()
{
	return m_container;
}

void ObjectValueContainer::GetObjectOfType(const TypeDef& typeDef, Value& container)
{
	ObjectValueContainer& inst = GetContainer();
	inst.CheckAccess();
	
	for (auto it = inst.m_typedValues.begin(); it != inst.m_typedValues.end(); ++it)
	{
		if (!TypeDef::IsA(*it->first, typeDef))
		{
			continue;
		}

		for (auto setIt = it->second.begin(); setIt != it->second.end(); ++setIt) {
			container.AssignObject(*setIt);
			return;
		}
	}
}

void ObjectValueContainer::Register(ObjectValue* value)
{
	CheckAccess();

	const TypeDef& type = value->GetTypeDef();
	m_typedValues[&type].insert(value);
}

void ObjectValueContainer::Unregister(ObjectValue* value)
{
	CheckAccess();

	const TypeDef& type = value->GetTypeDef();
	m_typedValues[&type].erase(value);
}

void ObjectValueContainer::GetObjectsOfType(const TypeDef& typeDef, std::list<ObjectValue*>& outObjects)
{
	CheckAccess();

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

void ObjectValueContainer::StartExclusiveThreadAccess()
{
	m_exclusiveThreadAccess = true;
	m_exclusiveThreadAccessId = std::this_thread::get_id();
}
