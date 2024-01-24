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
    m_moveSampler("33BEE8F2-486A-4761-B089-8CDC2F6A0298", TypeTypeDef::GetTypeDef(animation::BlendSpaceSamplerTypeDef::GetTypeDef()))
{
    {
        m_moveSampler.m_name = "Idle Sampler";
        m_moveSampler.m_category = "Setup";
        m_moveSampler.m_getValue = [](CompositeValue* obj) -> Value& {
            CharacterAnimator* animator = static_cast<CharacterAnimator*>(obj);
            return animator->m_moveSamplerDef;
        };
        m_properties[m_moveSampler.GetId()] = &m_moveSampler;
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
        const AssetTypeDef* asset = m_moveSamplerDef.GetType<const AssetTypeDef*>();
        asset->Construct(m_moveSampler);

        animation::BlendSpaceSampler* sampler = m_moveSampler.GetValue<animation::BlendSpaceSampler*>();

        jobs::RunAsync(jobs::Job::CreateByLambda([=]() {
            sampler->Load(done);
        }));
    });

    jobs::RunSync(init);
}

animation::PoseSampler* game::CharacterAnimator::GetSampler()
{
    return m_moveSampler.GetValue<animation::BlendSpaceSampler*>();
}

game::CharacterAnimator::CharacterAnimator(const ReferenceTypeDef& typeDef) :
    animation::Animator(typeDef),
    m_moveSamplerDef(CharacterAnimatorTypeDef::GetTypeDef().m_moveSampler.GetType(), this),
    m_moveSampler(animation::BlendSpaceSamplerTypeDef::GetTypeDef(), this)
{
}

game::CharacterAnimator::~CharacterAnimator()
{
}

void game::CharacterAnimator::Tick(double dt)
{
    using namespace math;

    const double transitionDuration = 0.2;

    Character* character = m_actor.GetValue<Character*>();
    if (m_lastTickWasFalling != character->m_jumpComponent.m_duringJump)
    {
        m_transitionTimeLeft = transitionDuration;
    }
    m_lastTickWasFalling = character->m_jumpComponent.m_duringJump;

    m_transitionTimeLeft -= dt;
    if (m_transitionTimeLeft < 0)
    {
        m_transitionTimeLeft = 0;
    }

    animation::BlendSpaceSampler* sampler = m_moveSampler.GetValue<animation::BlendSpaceSampler*>();
    animation::BlendSpaceSampler* moveSampler = sampler->m_sampler1.GetValue<animation::BlendSpaceSampler*>();
    animation::AnimationSampler* fallSampler = sampler->m_sampler2.GetValue<animation::AnimationSampler*>();

    sampler->m_alpha = m_transitionTimeLeft / transitionDuration;
    if (sampler->m_alpha < 0)
    {
        sampler->m_alpha = 0;
    }
    if (sampler->m_alpha > 1)
    {
        sampler->m_alpha = 1;
    }

    sampler->m_alpha = m_lastTickWasFalling ? 1 - sampler->m_alpha : sampler->m_alpha;

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
    moveSampler->m_alpha = blendFactor;

    animation::Animator::Tick(dt);
}

