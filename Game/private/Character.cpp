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
    MeshActor(typeDef),
    m_jumpComponent(*this)
{
}
game::Character::~Character()
{
}

game::JumpComponent::JumpComponent(Character& character) :
    m_character(character)
{
}

void game::JumpComponent::StartJump()
{
    if (m_duringJump)
    {
        return;
    }

    m_duringJump = true;
    m_jumpTime = 0;
}

void game::JumpComponent::UpdateJump(double dt)
{
    const float jumpDuration = 1;
    const float G = 9.8f;

    if (!m_duringJump)
    {
        return;
    }

    m_jumpTime += dt;
    if (m_jumpTime >= jumpDuration)
    {
        m_character.m_curTransform.m_position.m_coefs[1] = 0;
        m_duringJump = false;
        return;
    }

    m_character.m_curTransform.m_position.m_coefs[1] = -0.5 * G * m_jumpTime * m_jumpTime + 0.5 * G * m_jumpTime * jumpDuration;
}
