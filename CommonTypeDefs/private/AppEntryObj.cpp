#include "AppEntryObj.h"

app::AppEntryObj::AppEntryObj(const ReferenceTypeDef& appEntryTypeDef, const CompositeValue* outer) :
	ObjectValue(appEntryTypeDef, outer)
{
}

void app::AppEntryObj::Boot()
{
	TypeDef::SaveReflectionData();
}
