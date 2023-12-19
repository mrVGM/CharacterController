#include "Geometry.h"

#include "Mesh.h"
#include "GeometryAppEntry.h"

void geo::Boot()
{
	MeshTypeDef::GetTypeDef();
	GeometryAppEntryTypeDef::GetTypeDef();
}