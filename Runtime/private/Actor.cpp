#include "Actor.h"


#define THROW_ERROR(hRes, error) \
if (FAILED(hRes)) {\
    throw error;\
}

namespace
{
	BasicObjectContainer<runtime::ActorTypeDef> m_actorTypeDef;
}

const runtime::ActorTypeDef& runtime::ActorTypeDef::GetTypeDef()
{
	if (!m_actorTypeDef.m_object)
	{
		m_actorTypeDef.m_object = new ActorTypeDef();
	}

	return *m_actorTypeDef.m_object;
}

runtime::ActorTypeDef::ActorTypeDef() :
	ReferenceTypeDef(&ReferenceTypeDef::GetTypeDef(), "7DB63CF4-389B-4308-AE59-8A692AA1AC30")
{
	m_name = "Actor";
	m_category = "Scene";
}

runtime::ActorTypeDef::~ActorTypeDef()
{
}

void runtime::ActorTypeDef::Construct(Value& container) const
{
	throw "Can't Contruct a pure Actor!";
}

void runtime::Actor::Tick(double dt)
{
}

void runtime::Actor::PrepareForNextTick()
{
}

runtime::Actor::Actor(const ReferenceTypeDef& typeDef) :
	ObjectValue(typeDef)
{
	m_curTransform.m_position = math::Vector3{ 0, 0, 0 };
	m_curTransform.m_rotation = math::Vector4{ 1, 0, 0, 0 };
	m_curTransform.m_scale = math::Vector3{ 1, 1, 1 };
}

runtime::Actor::~Actor()
{
}


#undef THROW_ERROR