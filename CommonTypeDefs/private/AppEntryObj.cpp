#include "AppEntryObj.h"

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
