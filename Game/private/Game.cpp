#include "Game.h"

#include "PlayerController.h"
#include "Character.h"

void game::Boot()
{
	PlayerControllerTypeDef::GetTypeDef();
	CharacterTypeDef::GetTypeDef();
}
