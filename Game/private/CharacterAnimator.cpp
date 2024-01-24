#include "CharacterAnimator.h"

#include "PrimitiveTypes.h"
#include "AssetTypeDef.h"

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
    ReferenceTypeDef(&animation::AnimatorTypeDef::GetTypeDef(), "CD0F47C1-967C-4284-B58C-CBF1DA5D594C"),
    m_idleSampler("33BEE8F2-486A-4761-B089-8CDC2F6A0298", TypeTypeDef::GetTypeDef(animation::BlendSpaceSamplerTypeDef::GetTypeDef()))
{
    {
        m_idleSampler.m_name = "Idle Sampler";
        m_idleSampler.m_category = "Setup";
        m_idleSampler.m_getValue = [](CompositeValue* obj) -> Value& {
            CharacterAnimator* animator = static_cast<CharacterAnimator*>(obj);
            return animator->m_idleSamplerDef;
        };
        m_properties[m_idleSampler.GetId()] = &m_idleSampler;
    }

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

void game::CharacterAnimator::LoadData(jobs::Job* done)
{
    jobs::Job* init = jobs::Job::CreateByLambda([=]() {
        const AssetTypeDef* asset = m_idleSamplerDef.GetType<const AssetTypeDef*>();
        asset->Construct(m_idleSampler);

        animation::BlendSpaceSampler* sampler = m_idleSampler.GetValue<animation::BlendSpaceSampler*>();

        jobs::RunAsync(jobs::Job::CreateByLambda([=]() {
            sampler->Load(done);
        }));
    });

    jobs::RunSync(init);
}

animation::PoseSampler* game::CharacterAnimator::GetSampler()
{
    return m_idleSampler.GetValue<animation::BlendSpaceSampler*>();
}

game::CharacterAnimator::CharacterAnimator(const ReferenceTypeDef& typeDef) :
    animation::Animator(typeDef),
    m_idleSamplerDef(CharacterAnimatorTypeDef::GetTypeDef().m_idleSampler.GetType(), this),
    m_idleSampler(animation::BlendSpaceSamplerTypeDef::GetTypeDef(), this)
{
}

game::CharacterAnimator::~CharacterAnimator()
{
}

void game::CharacterAnimator::Tick(double dt)
{
    using namespace math;

    Character* character = m_actor.GetValue<Character*>();

    animation::BlendSpaceSampler* sampler = m_idleSampler.GetValue<animation::BlendSpaceSampler*>();

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

