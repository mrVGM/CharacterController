#include "CommonTypeDefs.h"

#include "AppEntry.h"
#include "MathStructs.h"

void common_defs::Boot()
{
	app::AppEntryTypeDef::GetTypeDef();
	common::Vector4TypeDef::GetTypeDef();
	common::Vector3TypeDef::GetTypeDef();
	common::TransformTypeDef::GetTypeDef();
}
