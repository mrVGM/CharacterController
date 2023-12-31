#include "Animator.h"

#include "TickUpdater.h"

#include "Jobs.h"

namespace
{
    BasicObjectContainer<animation::AnimatorTypeDef> m_animatorTypeDef;
}

const animation::AnimatorTypeDef& animation::AnimatorTypeDef::GetTypeDef()
{
    if (!m_animatorTypeDef.m_object)
    {
        m_animatorTypeDef.m_object = new AnimatorTypeDef();
    }

    return *m_animatorTypeDef.m_object;
}

animation::AnimatorTypeDef::AnimatorTypeDef() :
    ReferenceTypeDef(&runtime::TickUpdaterTypeDef::GetTypeDef(), "E072CB3C-7F9E-452E-AD27-88404A2E7997")
{
    m_name = "Animator";
    m_category = "Animation";
}

animation::AnimatorTypeDef::~AnimatorTypeDef()
{
}

void animation::AnimatorTypeDef::Construct(Value& value) const
{
    Animator* animator = new Animator(*this);
    value.AssignObject(animator);
}

void animation::Animator::LoadData(jobs::Job* done)
{
    jobs::RunSync(done);
}

animation::Animator::Animator(const ReferenceTypeDef& typeDef) :
    runtime::TickUpdater(typeDef)
{
}

animation::Animator::~Animator()
{
}

bool animation::Animator::IsTicking()
{
    return false;
}

void animation::Animator::Tick(double dt, jobs::Job* done)
{


    jobs::RunSync(done);
}


