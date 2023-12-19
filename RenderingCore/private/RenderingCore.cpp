#include "RenderingCore.h"

#include "WindowTypeDef.h"
#include "RenderingEntryTypeDef.h"
#include "DXShader.h"

void rendering::core::Boot()
{
	WindowTypeDef::GetTypeDef();
	RenderingEntryTypeDef::GetTypeDef();

	DXShaderTypeDef::GetTypeDef();
	DXVertexShaderTypeDef::GetTypeDef();
	DXPixelShaderTypeDef::GetTypeDef();
}