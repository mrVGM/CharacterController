#include "Runtime.h"

#include "TickUpdater.h"
#include "Actor.h"
#include "MeshActor.h"
#include "MeshBuffers.h"
#include "Animator.h"
#include "PlayerController.h"

void runtime::Boot()
{
	TickUpdaterTypeDef::GetTypeDef();
	ActorTypeDef::GetTypeDef();
	MeshActorTypeDef::GetTypeDef();
	MeshBuffersTypeDef::GetTypeDef();
	animation::AnimatorTypeDef::GetTypeDef();
	PlayerControllerTypeDef::GetTypeDef();
}
