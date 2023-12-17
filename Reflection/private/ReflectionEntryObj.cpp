#include "ReflectionEntryObj.h"

reflection::ReflectionEntryObj::ReflectionEntryObj(const ReflectionEntryTypeDef& reflectionEntryTypeDef, const CompositeValue* outer) :
	app::AppEntryObj(reflectionEntryTypeDef, outer)
{
}

void reflection::ReflectionEntryObj::Boot()
{
	TypeDef::SaveReflectionData();
}
