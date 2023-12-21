#include "ReflectionEntryObj.h"

reflection::ReflectionEntryObj::ReflectionEntryObj(const ReflectionEntryTypeDef& reflectionEntryTypeDef) :
	app::AppEntryObj(reflectionEntryTypeDef)
{
}

void reflection::ReflectionEntryObj::Boot()
{
	TypeDef::SaveReflectionData();
}
