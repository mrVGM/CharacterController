#include "AppObj.h"

#include "AppTypeDef.h"

app::AppObj::AppObj(const AppTypeDef& appTypedef, const CompositeValue* outer) :
	ObjectValue(appTypedef, outer),
	m_appEntry(AppTypeDef::GetTypeDef().m_appEntry.GetType(), this)
{
}
