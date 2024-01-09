#include "Geometry.h"

#include "Mesh.h"
#include "Skeleton.h"
#include "Animation.h"

void geo::Boot()
{
	MeshTypeDef::GetTypeDef();
	SkeletonTypeDef::GetTypeDef();
	AnimationTypeDef::GetTypeDef();
}