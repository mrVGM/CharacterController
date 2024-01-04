#include "Scene.h"

#include "SceneObject.h"
#include "SceneActor.h"

void scene::Boot()
{
	SceneObjectTypeDef::GetTypeDef();
	SceneActorTypeDef::GetTypeDef();
}
