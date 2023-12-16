#include "Types.h"

#include "PrimitiveTypes.h"
#include "CompositeTypeDef.h"
#include "GenericListDef.h"

void BootTypeSystem()
{
	BoolTypeDef::GetTypeDef();
	IntTypeDef::GetTypeDef();
	FloatTypeDef::GetTypeDef();
	StringTypeDef::GetTypeDef();
	GenericTypeDef::GetTypeDef();

	ValueTypeDef::GetTypeDef();
	ReferenceTypeDef::GetReferenceTypeDef();
	GenericListDef::GetTypeDef();
}
