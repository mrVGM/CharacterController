#include "PoseSampler.h"

#include "PrimitiveTypes.h"

#include "Animation.h"

#include "Jobs.h"

#include "ObjectValueContainer.h"

namespace
{
    BasicObjectContainer<animation::PoseSamplerTypeDef> m_poseSamplerTypeDef;
    BasicObjectContainer<animation::AnimationSamplerTypeDef> m_animationSamplerTypeDef;
}

const animation::PoseSamplerTypeDef& animation::PoseSamplerTypeDef::GetTypeDef()
{
    if (!m_poseSamplerTypeDef.m_object)
    {
        m_poseSamplerTypeDef.m_object = new PoseSamplerTypeDef();
    }

    return *m_poseSamplerTypeDef.m_object;
}

const animation::AnimationSamplerTypeDef& animation::AnimationSamplerTypeDef::GetTypeDef()
{
    if (!m_animationSamplerTypeDef.m_object)
    {
        m_animationSamplerTypeDef.m_object = new AnimationSamplerTypeDef();
    }

    return *m_animationSamplerTypeDef.m_object;
}

animation::PoseSamplerTypeDef::PoseSamplerTypeDef() :
    ReferenceTypeDef(&ReferenceTypeDef::GetTypeDef(), "042DF982-4850-4D79-8BD0-0A05B5405AC3")
{
    m_name = "Pose Sampler";
    m_category = "Animations";
}

animation::PoseSamplerTypeDef::~PoseSamplerTypeDef()
{
}

void animation::PoseSamplerTypeDef::Construct(Value& value) const
{
    throw "Can't construct pure Pose Sampler!";
}

animation::PoseSampler::PoseSampler(const ReferenceTypeDef& type) :
    ObjectValue(type)
{
}

animation::PoseSampler::~PoseSampler()
{
}

animation::AnimationSamplerTypeDef::AnimationSamplerTypeDef() :
    ReferenceTypeDef(&PoseSamplerTypeDef::GetTypeDef(), "18F7E46A-6EBE-4420-B2FC-322605091580"),
    m_animation("277F8B81-E6EC-45E9-9856-149F2FAA5B96", TypeTypeDef::GetTypeDef(geo::AnimationTypeDef::GetTypeDef()))
{
    {
        m_animation.m_name = "Animation";
        m_animation.m_category = "Setup";
        m_animation.m_getValue = [](CompositeValue* obj) -> Value&{
            AnimationSampler* animSampler = static_cast<AnimationSampler*>(obj);
            return animSampler->m_animationDef;
        };
        m_properties[m_animation.GetId()] = &m_animation;
    }

    m_name = "Animation Sampler";
    m_category = "Animations";
}

animation::AnimationSamplerTypeDef::~AnimationSamplerTypeDef()
{
}

void animation::AnimationSamplerTypeDef::Construct(Value& container) const
{
    AnimationSampler* sampler = new AnimationSampler(*this);
    container.AssignObject(sampler);
}

math::Matrix animation::AnimationSampler::SampleAnimation(double time, const std::string& bone, const geo::Skeleton& skeleton)
{
    geo::Animation* animation = m_animation.GetValue<geo::Animation*>();
    const geo::Animation::AnimChannel* animChannel = animation->GetAnimChannel(bone);

    if (!animChannel)
    {
        return skeleton.m_bindPose[skeleton.GetBoneIndex(bone)];
    }

    return animation->SampleChannel(time, *animChannel);
}

void animation::AnimationSampler::LoadData(jobs::Job* done)
{
    jobs::Job* load = jobs::Job::CreateByLambda([=]() {
        geo::Animation* anim = m_animation.GetValue<geo::Animation*>();
        anim->Load(done);
    });

    jobs::Job* init = jobs::Job::CreateByLambda([=]() {
        const TypeDef* animDef = m_animationDef.GetType<const TypeDef*>();
        ObjectValueContainer::GetObjectOfType(*animDef, m_animation);

        jobs::RunAsync(load);
    });

    jobs::RunSync(init);
}

animation::AnimationSampler::AnimationSampler(const ReferenceTypeDef& type) :
    PoseSampler(type),
    m_animationDef(animation::AnimationSamplerTypeDef::GetTypeDef().m_animation.GetType(), this),
    m_animation(geo::AnimationTypeDef::GetTypeDef(), this)
{
}

animation::AnimationSampler::~AnimationSampler()
{
}

const math::Matrix& animation::AnimationSampler::SampleTransform(
    const animation::Animator& animator,
    const geo::Skeleton& skeleton,
    const std::string& bone,
    double time)
{
    using namespace math;

    geo::Animation* anim = m_animation.GetValue<geo::Animation*>();

    math::Matrix mat = math::Matrix::GetIdentityMatrix();

    int curIndex = skeleton.GetBoneIndex(bone);
    while (curIndex >= 0)
    {
        const std::string& curBone = skeleton.m_boneNames[curIndex];
        if (!anim)
        {
            mat = skeleton.m_bindPose[curIndex] * mat;
        }
        else
        {
            mat = SampleAnimation(time, curBone, skeleton) * mat;
        }
        curIndex = skeleton.m_boneParents[curIndex];
    }

    return mat;
}
