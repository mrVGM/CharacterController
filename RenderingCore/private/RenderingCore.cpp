#include "RenderingCore.h"

#include "WindowTypeDef.h"
#include "RenderingEntryTypeDef.h"

void rendering::core::Boot()
{
	WindowTypeDef::GetTypeDef();
	RenderingEntryTypeDef::GetTypeDef();
}