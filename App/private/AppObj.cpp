#include "AppObj.h"

#include "AppTypeDef.h"

app::AppObj::AppObj(const AppTypeDef& appTypedef) :
	ObjectValue(appTypedef),
	m_appEntry(AppTypeDef::GetTypeDef().m_appEntry.GetType(), this)
{
}

app::AppObj::~AppObj()
{
}
