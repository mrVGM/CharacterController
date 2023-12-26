#include "TickUpdater.h"

namespace
{
	BasicObjectContainer<rendering::renderer::TickUpdaterTypeDef> m_tickUpdater;
}

const rendering::renderer::TickUpdaterTypeDef& rendering::renderer::TickUpdaterTypeDef::GetTypeDef()
{
	if (!m_tickUpdater.m_object)
	{
		m_tickUpdater.m_object = new TickUpdaterTypeDef();
	}

	return *m_tickUpdater.m_object;
}

rendering::renderer::TickUpdaterTypeDef::TickUpdaterTypeDef() :
	ReferenceTypeDef(&ReferenceTypeDef::GetTypeDef(), "C01E1931-B22B-4CE1-8F97-D895200C2A87")
{
	m_name = "Tick Updater";
	m_category = "Renderer";
}

rendering::renderer::TickUpdaterTypeDef::~TickUpdaterTypeDef()
{
}

void rendering::renderer::TickUpdaterTypeDef::Construct(Value& container) const
{
	throw "Can't construct Pure Tick Updater!";
}

rendering::renderer::TickUpdater::TickUpdater(const ReferenceTypeDef& typeDef) :
	ObjectValue(typeDef)
{
}

rendering::renderer::TickUpdater::~TickUpdater()
{
}
