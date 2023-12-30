#include "TickUpdater.h"

namespace
{
	BasicObjectContainer<runtime::TickUpdaterTypeDef> m_tickUpdater;
}

const runtime::TickUpdaterTypeDef& runtime::TickUpdaterTypeDef::GetTypeDef()
{
	if (!m_tickUpdater.m_object)
	{
		m_tickUpdater.m_object = new TickUpdaterTypeDef();
	}

	return *m_tickUpdater.m_object;
}

runtime::TickUpdaterTypeDef::TickUpdaterTypeDef() :
	ReferenceTypeDef(&ReferenceTypeDef::GetTypeDef(), "C01E1931-B22B-4CE1-8F97-D895200C2A87")
{
	m_name = "Tick Updater";
	m_category = "Renderer";
}

runtime::TickUpdaterTypeDef::~TickUpdaterTypeDef()
{
}

void runtime::TickUpdaterTypeDef::Construct(Value& container) const
{
	throw "Can't construct Pure Tick Updater!";
}

runtime::TickUpdater::TickUpdater(const ReferenceTypeDef& typeDef) :
	ObjectValue(typeDef)
{
}

runtime::TickUpdater::~TickUpdater()
{
}
