#pragma once

#include "TypeDef.h"
#include "CompositeValue.h"

#include <map>
#include <set>
#include <list>

class ObjectValueContainer
{
private:
	static ObjectValueContainer m_container;
	
	std::map<const TypeDef*, std::set<ObjectValue*>> m_typedValues;

	ObjectValueContainer();

public:
	static ObjectValueContainer& GetContainer();

	void Register(ObjectValue* value);
	void Unregister(ObjectValue* value);

	void GetObjectsOfType(const TypeDef& typeDef, std::list<ObjectValue*>& outObjects);
};
