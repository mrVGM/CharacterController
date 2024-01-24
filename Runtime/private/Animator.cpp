#include "Animator.h"

#include "PrimitiveTypes.h"

#include "TickUpdater.h"

#include "DXMutableBuffer.h"
#include "Skeleton.h"

#include "ObjectValueContainer.h"

#include "PoseSampler.h"

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
    ReferenceTypeDef(&ReferenceTypeDef::GetTypeDef(), "E072CB3C-7F9E-452E-AD27-88404A2E7997")
{
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

void animation::Animator::LoadData(jobs::Job* done)
{
    jobs::RunSync(done);
}

animation::Animator::Animator(const ReferenceTypeDef& typeDef) :
    ObjectValue(typeDef),
    m_actor(runtime::MeshActorTypeDef::GetTypeDef(), this)
{
}

animation::Animator::~Animator()
{
}

animation::PoseSampler* animation::Animator::GetSampler()
{
    return nullptr;
}

bool animation::Animator::IsTicking() const
{
    runtime::MeshActor* actor = m_actor.GetValue<runtime::MeshActor*>();
    return actor;
}

void animation::Animator::Tick(double dt)
{
    m_curTime += 1 * dt;

    runtime::MeshActor* actor = m_actor.GetValue<runtime::MeshActor*>();
    rendering::DXMutableBuffer* poseBuffer = actor->m_poseBuffer.GetValue<rendering::DXMutableBuffer*>();
    rendering::DXBuffer* uploadBuff = poseBuffer->m_uploadBuffer.GetValue<rendering::DXBuffer*>();

    geo::Mesh* mesh = actor->m_mesh.GetValue<geo::Mesh*>();
    const geo::Mesh::SkinData& skinData = mesh->m_skinData;

    geo::Skeleton* skeleton = actor->m_skeleton.GetValue<geo::Skeleton*>();

    math::Matrix* poseData = static_cast<math::Matrix*>(uploadBuff->Map());

    animation::PoseSampler* sampler = GetSampler();

    for (auto it = skinData.m_boneNames.begin(); it != skinData.m_boneNames.end(); ++it)
    {
        math::Matrix mat = math::Matrix::GetIdentityMatrix();
        if (sampler)
        {
            mat = sampler->SampleTransform(*this, *skeleton, *it, m_curTime);
        }
        else
        {
            int curIndex = skeleton->GetBoneIndex(*it);
            while (curIndex >= 0)
            {
                mat = skeleton->m_bindPose[curIndex] * mat;
                curIndex = skeleton->m_boneParents[curIndex];
            }
        }

        math::Matrix tmp = mat.Transpose();
        *(poseData++) = tmp;
    }

    uploadBuff->Unmap();
    poseBuffer->SetDirty(true);
}

void animation::Animator::SetActor(runtime::MeshActor& actor)
{
    m_actor.AssignObject(&actor);
}
