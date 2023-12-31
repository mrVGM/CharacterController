#include "Animator.h"

#include "TickUpdater.h"

#include "DXMutableBuffer.h"
#include "Skeleton.h"

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
    runtime::TickUpdater(typeDef),
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
    runtime::Actor* actor = m_actor.GetValue<runtime::Actor*>();
    rendering::DXMutableBuffer* poseBuffer = actor->m_poseBuffer.GetValue<rendering::DXMutableBuffer*>();
    rendering::DXBuffer* uploadBuff = poseBuffer->m_uploadBuffer.GetValue<rendering::DXBuffer*>();

    geo::Mesh* mesh = actor->m_mesh.GetValue<geo::Mesh*>();
    const geo::Mesh::SkinData& skinData = mesh->m_skinData;

    geo::Skeleton* skeleton = actor->m_skeleton.GetValue<geo::Skeleton*>();

    math::Matrix* poseData = static_cast<math::Matrix*>(uploadBuff->Map());

    for (auto it = skinData.m_boneNames.begin(); it != skinData.m_boneNames.end(); ++it)
    {
        math::Matrix mat = math::Matrix::GetIdentityMatrix();

        int curIndex = skeleton->GetBoneIndex(*it);
        while (curIndex >= 0)
        {
            mat = skeleton->m_bindPose[curIndex] * mat;
            curIndex = skeleton->m_boneParents[curIndex];
        }

        *(poseData++) = mat.Transpose();
    }

    uploadBuff->Unmap();
    poseBuffer->SetDirty(true);

    jobs::RunSync(done);
}

void animation::Animator::SetActor(runtime::Actor& actor)
{
    m_actor.AssignObject(&actor);
}
