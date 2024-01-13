#include "Character.h"

namespace
{
    BasicObjectContainer<game::CharacterTypeDef> m_character;
}

const game::CharacterTypeDef& game::CharacterTypeDef::GetTypeDef()
{
    if (!m_character.m_object)
    {
        m_character.m_object = new CharacterTypeDef();
    }

    return *m_character.m_object;
}

game::CharacterTypeDef::CharacterTypeDef() :
    ReferenceTypeDef(&runtime::MeshActorTypeDef::GetTypeDef(), "F1F20B69-C156-4110-B17E-A302B41F7434")
{
    m_name = "Character";
    m_category = "Game";
}

game::CharacterTypeDef::~CharacterTypeDef()
{
}

void game::CharacterTypeDef::Construct(Value& value) const
{
    Character* pc = new Character(*this);
    value.AssignObject(pc);
}

game::Character::Character(const ReferenceTypeDef& typeDef) :
    MeshActor(typeDef)
{
}
game::Character::~Character()
{
}
