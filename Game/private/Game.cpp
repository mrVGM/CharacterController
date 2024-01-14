#include "Game.h"

#include "PlayerController.h"
#include "Character.h"
#include "CharacterAnimator.h"

void game::Boot()
{
	PlayerControllerTypeDef::GetTypeDef();
	CharacterTypeDef::GetTypeDef();
	CharacterAnimatorTypeDef::GetTypeDef();
}
