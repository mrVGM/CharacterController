#include "Scene.h"

#include "SceneObject.h"
#include "Actor.h"

void scene::Boot()
{
	SceneObjectTypeDef::GetTypeDef();
	ActorTypeDef::GetTypeDef();
}
