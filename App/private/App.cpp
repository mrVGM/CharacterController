#include "App.h"

#include "AppTypeDef.h"
#include "AppEntryTypeDef.h"

void app::Boot()
{
	AppTypeDef::GetAppTypeDef();
	AppEntryTypeDef::GetAppEntryTypeDef();
}
