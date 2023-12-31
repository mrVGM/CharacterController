#include "Runtime.h"

#include "TickUpdater.h"
#include "Actor.h"
#include "MeshBuffers.h"

void runtime::Boot()
{
	TickUpdaterTypeDef::GetTypeDef();
	ActorTypeDef::GetTypeDef();
	MeshBuffersTypeDef::GetTypeDef();
}
