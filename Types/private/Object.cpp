#include "Object.h"

Object::Object(const ReferenceType& typeDef) :
	m_typeDef(typeDef)
{
}

ObjectValue::ObjectValue() :
	m_containerSet(false)
{
}

void ObjectValue::SetContainer(const Object* container)
{
	if (m_containerSet)
	{
		throw "Can't change Container!";
	}

	m_container = container;
	m_containerSet = true;
}

ObjectValue::ObjectValue(const Object* container) :
	m_container(container)
{
}

ObjectValue& ObjectValue::operator=(const ObjectValue& other)
{
	if (!m_containerSet)
	{
		throw "Bad Value!";
	}

	m_object = other.m_object;

	return *this;
}

ObjectValue::~ObjectValue()
{
}
