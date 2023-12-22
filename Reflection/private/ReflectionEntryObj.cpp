#include "ReflectionEntryObj.h"

reflection::ReflectionEntryObj::ReflectionEntryObj(const ReflectionEntryTypeDef& reflectionEntryTypeDef) :
	app::AppEntryObj(reflectionEntryTypeDef)
{
}

reflection::ReflectionEntryObj::~ReflectionEntryObj()
{
}

void reflection::ReflectionEntryObj::Boot()
{
	TypeDef::SaveReflectionData();
}
