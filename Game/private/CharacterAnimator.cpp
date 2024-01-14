#include "CharacterAnimator.h"

#include "Animator.h"
#include "PoseSampler.h"
#include "Character.h"

#include "Jobs.h"

#include "ObjectValueContainer.h"

namespace
{
    BasicObjectContainer<game::CharacterAnimatorTypeDef> m_characterAnimatorTypeDef;
}

const game::CharacterAnimatorTypeDef& game::CharacterAnimatorTypeDef::GetTypeDef()
{
    if (!m_characterAnimatorTypeDef.m_object)
    {
        m_characterAnimatorTypeDef.m_object = new CharacterAnimatorTypeDef();
    }

    return *m_characterAnimatorTypeDef.m_object;
}

game::CharacterAnimatorTypeDef::CharacterAnimatorTypeDef() :
    ReferenceTypeDef(&animation::AnimatorTypeDef::GetTypeDef(), "CD0F47C1-967C-4284-B58C-CBF1DA5D594C")
{
    m_name = "Character Animator";
    m_category = "Game";
}

game::CharacterAnimatorTypeDef::~CharacterAnimatorTypeDef()
{
}

void game::CharacterAnimatorTypeDef::Construct(Value& value) const
{
    CharacterAnimator* animator = new CharacterAnimator(*this);
    value.AssignObject(animator);
}

game::CharacterAnimator::CharacterAnimator(const ReferenceTypeDef& typeDef) :
    animation::Animator(typeDef)
{
}

game::CharacterAnimator::~CharacterAnimator()
{
}

void game::CharacterAnimator::Tick(double dt)
{
    using namespace math;

    Character* character = m_actor.GetValue<Character*>();

    animation::BlendSpaceSampler* sampler = m_sampler.GetValue<animation::BlendSpaceSampler*>();

    float speed = Dot(character->m_velocity, character->m_velocity);
    speed = sqrt(speed);

    float blendFactor = speed / 6;

    if (blendFactor < 0)
    {
        blendFactor = 0;
    }
    if (blendFactor > 1)
    {
        blendFactor = 1;
    }
    sampler->m_alpha = blendFactor;

    animation::Animator::Tick(dt);
}

