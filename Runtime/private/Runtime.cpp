#include "Runtime.h"

#include "TickUpdater.h"
#include "Actor.h"
#include "MeshActor.h"
#include "MeshBuffers.h"
#include "Animator.h"

void runtime::Boot()
{
	TickUpdaterTypeDef::GetTypeDef();
	ActorTypeDef::GetTypeDef();
	MeshActorTypeDef::GetTypeDef();
	MeshBuffersTypeDef::GetTypeDef();
	animation::AnimatorTypeDef::GetTypeDef();
}
