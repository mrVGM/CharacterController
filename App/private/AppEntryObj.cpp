#include "AppEntryObj.h"

app::AppEntryObj::AppEntryObj(const AppEntryTypeDef& appEntryTypeDef, const CompositeValue* outer) :
	ObjectValue(appEntryTypeDef, outer)
{
}

void app::AppEntryObj::Boot()
{
}
