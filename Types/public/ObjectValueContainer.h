#pragma once

#include "TypeDef.h"
#include "CompositeValue.h"

#include <map>
#include <set>
#include <list>
#include <thread>

class ObjectValueContainer
{
private:
	static ObjectValueContainer m_container;
	
	std::map<const TypeDef*, std::set<ObjectValue*>> m_typedValues;

	ObjectValueContainer();

	bool m_exclusiveThreadAccess = false;
	std::thread::id m_exclusiveThreadAccessId;

	void CheckAccess();

public:
	static ObjectValueContainer& GetContainer();
	static void GetObjectOfType(const TypeDef& typeDef, Value& container);

	void Register(ObjectValue* value);
	void Unregister(ObjectValue* value);

	void GetObjectsOfType(const TypeDef& typeDef, std::list<ObjectValue*>& outObjects);

	void StartExclusiveThreadAccess();
};
