#include "Types.h"

#include "PrimitiveTypes.h"
#include "GenericListDef.h"

void BootTypeSystem()
{
	BoolTypeDef::GetTypeDef();
	GenericListDef::GetTypeDef();
}
