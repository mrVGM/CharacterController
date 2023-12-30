#include "Geometry.h"

#include "Mesh.h"
#include "Skeleton.h"
#include "GeometryAppEntry.h"

void geo::Boot()
{
	MeshTypeDef::GetTypeDef();
	SkeletonTypeDef::GetTypeDef();
	GeometryAppEntryTypeDef::GetTypeDef();
}