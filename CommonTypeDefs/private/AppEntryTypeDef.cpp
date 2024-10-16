#include "AppEntry.h"

namespace
{
	BasicObjectContainer<app::AppEntryTypeDef> m_reflectionEntryTypeDef;
}

const app::AppEntryTypeDef& app::AppEntryTypeDef::GetTypeDef()
{
	if (!m_reflectionEntryTypeDef.m_object)
	{
		m_reflectionEntryTypeDef.m_object = new AppEntryTypeDef();
	}

	return *m_reflectionEntryTypeDef.m_object;
}

app::AppEntryTypeDef::AppEntryTypeDef() :
	ReferenceTypeDef(&ReferenceTypeDef::GetTypeDef(), "7D79EC02-6FBC-48F3-BB6F-1C07BE762DE8")
{
	m_name = "App Entry";
	m_category = "App";
}

app::AppEntryTypeDef::~AppEntryTypeDef()
{
}

void app::AppEntryTypeDef::Construct(Value& container) const
{
	AppEntryObj* obj = new AppEntryObj(*this);
	container.AssignObject(obj);
}


app::AppEntryObj::AppEntryObj(const ReferenceTypeDef& appEntryTypeDef) :
	ObjectValue(appEntryTypeDef)
{
}

app::AppEntryObj::~AppEntryObj()
{
}

void app::AppEntryObj::Boot()
{
	TypeDef::SaveReflectionData();
}