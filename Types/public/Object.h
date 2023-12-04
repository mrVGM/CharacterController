#pragma once

class ReferenceType;

class Object;

class ObjectValue
{
private:
	bool m_containerSet = true;
	const Object* m_container = nullptr;
	Object* m_object = nullptr;

public:
	ObjectValue();
	ObjectValue(const Object* container);
	ObjectValue& operator=(const ObjectValue& other);
	~ObjectValue();

	void SetContainer(const Object* container);
};

class Object
{
private:
	const ReferenceType& m_typeDef;

public:
	Object(const ReferenceType& typeDef);
};

