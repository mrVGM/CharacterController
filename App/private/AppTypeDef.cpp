#include "AppTypeDef.h"

#include "PrimitiveTypes.h"
#include "AppObj.h"
#include "AppEntryTypeDef.h"

namespace
{
	const char* m_appTypeId = "6CC559E5-8408-4894-8580-D2963791F201";

	BasicObjectContainer<app::AppTypeDef> m_appTypeDef;
}

app::AppTypeDef::AppTypeDef() :
	ReferenceTypeDef(&ReferenceTypeDef::GetReferenceTypeDef(), m_appTypeId),
	m_appEntry("A9E68EEE-504B-44B2-B3FF-6EF65493D3E9", TypeTypeDef::GetTypeDef(AppEntryTypeDef::GetAppEntryTypeDef()))
{
	m_name = "App Type";
	m_category = "App";

	m_appEntry.m_name = "App Entry Point";
	m_appEntry.m_category = "Setup";
	m_appEntry.m_getValue = [](CompositeValue* obj) -> Value& {
		AppObj* appObj = static_cast<AppObj*>(obj);
		return appObj->m_appEntry;
	};

	m_properties[m_appEntry.GetId()] = &m_appEntry;
}

const app::AppTypeDef& app::AppTypeDef::GetAppTypeDef()
{
	if (!m_appTypeDef.m_object)
	{
		m_appTypeDef.m_object = new AppTypeDef();
	}

	return *m_appTypeDef.m_object;
}
