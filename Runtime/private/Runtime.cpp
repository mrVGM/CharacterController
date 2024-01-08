#include "Runtime.h"

#include "TickUpdater.h"
#include "Actor.h"
#include "MeshBuffers.h"
#include "Animator.h"
#include "PlayerController.h"

void runtime::Boot()
{
	TickUpdaterTypeDef::GetTypeDef();
	ActorTypeDef::GetTypeDef();
	MeshBuffersTypeDef::GetTypeDef();
	animation::AnimatorTypeDef::GetTypeDef();
	PlayerControllerTypeDef::GetTypeDef();
}
