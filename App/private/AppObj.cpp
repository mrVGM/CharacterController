#include "AppObj.h"

#include "AppTypeDef.h"

app::AppObj::AppObj(const AppTypeDef& appTypedef, const CompositeValue* outer) :
	ObjectValue(appTypedef, outer),
	m_appEntry(AppTypeDef::GetAppTypeDef().m_appEntry.GetType(), this)
{
}
