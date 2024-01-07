#include "Animator.h"

#include "PrimitiveTypes.h"

#include "TickUpdater.h"

#include "DXMutableBuffer.h"
#include "Skeleton.h"

#include "ObjectValueContainer.h"

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
    ReferenceTypeDef(&runtime::TickUpdaterTypeDef::GetTypeDef(), "E072CB3C-7F9E-452E-AD27-88404A2E7997"),
    m_idle("719574BD-6DC2-434A-BD56-D7891C7C4983", TypeTypeDef::GetTypeDef(geo::AnimationTypeDef::GetTypeDef()))
{
    {
        m_idle.m_name = "Idle Anim";
        m_idle.m_category = "Setup";
        m_idle.m_getValue = [](CompositeValue* obj) -> Value& {
            Animator* animator = static_cast<Animator*>(obj);
            return animator->m_idleDef;
        };
        m_properties[m_idle.GetId()] = &m_idle;
    }

    m_name = "Animator";
    m_category = "Animations";
}

animation::AnimatorTypeDef::~AnimatorTypeDef()
{
}

void animation::AnimatorTypeDef::Construct(Value& value) const
{
    Animator* animator = new Animator(*this);
    value.AssignObject(animator);
}

const math::Matrix& animation::Animator::SampleTransform(double time, const std::string& bone, const geo::Animation& animation)
{
    const geo::Animation::AnimChannel* animChannel = animation.GetAnimChannel(bone);

    if (!animChannel)
    {
        runtime::Actor* actor = m_actor.GetValue<runtime::Actor*>();
        geo::Skeleton* skeleton = actor->m_skeleton.GetValue<geo::Skeleton*>();

        return skeleton->m_bindPose[skeleton->GetBoneIndex(bone)];
    }

    return animation.SampleChannel(time, *animChannel);
}

void animation::Animator::LoadData(jobs::Job* done)
{
    jobs::Job* load = jobs::Job::CreateByLambda([=]() {
        geo::Animation* idle = m_idle.GetValue<geo::Animation*>();
        if (!idle)
        {
            jobs::RunSync(done);
            return;
        }

        idle->Load(done);
    });

    jobs::Job* init = jobs::Job::CreateByLambda([=]() {
        const TypeDef* idleDef = m_idleDef.GetType<const TypeDef*>();
        if (idleDef)
        {
            m_idle.AssignObject(ObjectValueContainer::GetObjectOfType(*idleDef));
        }
        jobs::RunAsync(load);
    });

    jobs::RunSync(init);
}

animation::Animator::Animator(const ReferenceTypeDef& typeDef) :
    runtime::TickUpdater(typeDef),

    m_idleDef(AnimatorTypeDef::GetTypeDef().m_idle.GetType(), this),
    m_idle(geo::AnimationTypeDef::GetTypeDef(), this),

    m_actor(runtime::ActorTypeDef::GetTypeDef(), this)
{
}

animation::Animator::~Animator()
{
}

bool animation::Animator::IsTicking()
{
    runtime::Actor* actor = m_actor.GetValue<runtime::Actor*>();
    return actor;
}

void animation::Animator::Tick(double dt, jobs::Job* done)
{
    m_curTime += 0.05 * dt;

    runtime::Actor* actor = m_actor.GetValue<runtime::Actor*>();
    rendering::DXMutableBuffer* poseBuffer = actor->m_poseBuffer.GetValue<rendering::DXMutableBuffer*>();
    rendering::DXBuffer* uploadBuff = poseBuffer->m_uploadBuffer.GetValue<rendering::DXBuffer*>();

    geo::Mesh* mesh = actor->m_mesh.GetValue<geo::Mesh*>();
    const geo::Mesh::SkinData& skinData = mesh->m_skinData;

    geo::Skeleton* skeleton = actor->m_skeleton.GetValue<geo::Skeleton*>();

    math::Matrix* poseData = static_cast<math::Matrix*>(uploadBuff->Map());

    geo::Animation* anim = m_idle.GetValue<geo::Animation*>();

    for (auto it = skinData.m_boneNames.begin(); it != skinData.m_boneNames.end(); ++it)
    {
        math::Matrix mat = math::Matrix::GetIdentityMatrix();

        int curIndex = skeleton->GetBoneIndex(*it);
        while (curIndex >= 0)
        {
            const std::string& curBone = skeleton->m_boneNames[curIndex];
            if (!anim)
            {
                mat = skeleton->m_bindPose[curIndex] * mat;
            }
            else
            {
                mat = SampleTransform(m_curTime, curBone, *anim) * mat;
            }
            curIndex = skeleton->m_boneParents[curIndex];
        }

        math::Matrix tmp = mat.Transpose();
        *(poseData++) = tmp;
    }

    uploadBuff->Unmap();
    poseBuffer->SetDirty(true);

    jobs::RunSync(done);
}

void animation::Animator::SetActor(runtime::Actor& actor)
{
    m_actor.AssignObject(&actor);
}
