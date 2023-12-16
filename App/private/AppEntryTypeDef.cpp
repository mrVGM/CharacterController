#include "AppEntryTypeDef.h"
#include "AppEntryObj.h"

namespace
{
	BasicObjectContainer<app::AppEntryTypeDef> m_appEntryTypeDef;
}

const app::AppEntryTypeDef& app::AppEntryTypeDef::GetAppEntryTypeDef()
{
	if (!m_appEntryTypeDef.m_object)
	{
		m_appEntryTypeDef.m_object = new AppEntryTypeDef();
	}

	return *m_appEntryTypeDef.m_object;
}

app::AppEntryTypeDef::AppEntryTypeDef() :
	ReferenceTypeDef(&ReferenceTypeDef::GetReferenceTypeDef(), "7D79EC02-6FBC-48F3-BB6F-1C07BE762DE8")
{
	m_name = "App Entry";
	m_category = "App";
}

void app::AppEntryTypeDef::Construct(Value& container) const
{
	AppEntryObj* obj = new AppEntryObj(*this, nullptr);
	container.AssignObject(obj);
}
