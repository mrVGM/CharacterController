#include "AppEntryObj.h"

app::AppEntryObj::AppEntryObj(const ReferenceTypeDef& appEntryTypeDef) :
	ObjectValue(appEntryTypeDef)
{
}

void app::AppEntryObj::Boot()
{
	TypeDef::SaveReflectionData();
}
