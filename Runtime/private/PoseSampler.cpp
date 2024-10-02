#include "PoseSampler.h"

#include "PrimitiveTypes.h"
#include "AssetTypeDef.h"

#include "Animation.h"

#include "Jobs.h"

#include "ObjectValueContainer.h"

namespace
{
    BasicObjectContainer<animation::PoseSamplerTypeDef> m_poseSamplerTypeDef;
    BasicObjectContainer<animation::AnimationSamplerTypeDef> m_animationSamplerTypeDef;
    BasicObjectContainer<animation::BlendSpaceSamplerTypeDef> m_blendSpaceSamplerTypeDef;
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

const animation::BlendSpaceSamplerTypeDef& animation::BlendSpaceSamplerTypeDef::GetTypeDef()
{
    if (!m_blendSpaceSamplerTypeDef.m_object)
    {
        m_blendSpaceSamplerTypeDef.m_object = new BlendSpaceSamplerTypeDef();
    }

    return *m_blendSpaceSamplerTypeDef.m_object;
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

void animation::AnimationSampler::LoadData(jobs::Job done)
{
    jobs::Job load = [=]() {
        geo::Animation* anim = m_animation.GetValue<geo::Animation*>();
        anim->Load(done);
    };

    jobs::Job init = [=]() {
        const TypeDef* animDef = m_animationDef.GetType<const TypeDef*>();
        ObjectValueContainer::GetObjectOfType(*animDef, m_animation);

        jobs::RunAsync(load);
    };

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

    time -= m_timeOffset;

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



animation::BlendSpaceSamplerTypeDef::BlendSpaceSamplerTypeDef() :
    ReferenceTypeDef(&PoseSamplerTypeDef::GetTypeDef(), "BA8782B7-9E39-4003-AE28-793EF980F280"),
    m_sampler1("7CC14CDF-13DF-4223-BF23-84889F308AA4", TypeTypeDef::GetTypeDef(animation::PoseSamplerTypeDef::GetTypeDef())),
    m_sampler2("3CFABBE1-5A43-4FE5-9275-92F98863C6E9", TypeTypeDef::GetTypeDef(animation::PoseSamplerTypeDef::GetTypeDef()))
{
    {
        m_sampler1.m_name = "Sampler 1";
        m_sampler1.m_category = "Setup";
        m_sampler1.m_getValue = [](CompositeValue* obj) -> Value& {
            BlendSpaceSampler* sampler = static_cast<BlendSpaceSampler*>(obj);
            return sampler->m_sampler1Def;
        };
        m_properties[m_sampler1.GetId()] = &m_sampler1;
    }

    {
        m_sampler2.m_name = "Sampler 2";
        m_sampler2.m_category = "Setup";
        m_sampler2.m_getValue = [](CompositeValue* obj) -> Value& {
            BlendSpaceSampler* sampler = static_cast<BlendSpaceSampler*>(obj);
            return sampler->m_sampler2Def;
        };
        m_properties[m_sampler2.GetId()] = &m_sampler2;
    }

    m_name = "Blend Space Sampler";
    m_category = "Animations";
}

animation::BlendSpaceSamplerTypeDef::~BlendSpaceSamplerTypeDef()
{
}

void animation::BlendSpaceSamplerTypeDef::Construct(Value& container) const
{
    BlendSpaceSampler* sampler = new BlendSpaceSampler(*this);
    container.AssignObject(sampler);
}

void animation::BlendSpaceSampler::LoadData(jobs::Job done)
{
    int* toLoad = new int;
    *toLoad = 0;

    auto loaded = [=]() {
        --(*toLoad);
        if (*toLoad > 0)
        {
            return;
        }

        delete toLoad;

        jobs::RunSync(done);
    };

    jobs::Job init = [=]() {
        const AssetTypeDef* samplerDef1 = m_sampler1Def.GetType<const AssetTypeDef*>();
        const AssetTypeDef* samplerDef2 = m_sampler2Def.GetType<const AssetTypeDef*>();

        samplerDef1->Construct(m_sampler1);
        samplerDef2->Construct(m_sampler2);

        PoseSampler* sampler1 = m_sampler1.GetValue<PoseSampler*>();
        PoseSampler* sampler2 = m_sampler2.GetValue<PoseSampler*>();

        (*toLoad)++;
        jobs::RunAsync([=]() {
            sampler1->Load(loaded);
        });

        (*toLoad)++;
        jobs::RunAsync([=]() {
            sampler2->Load(loaded);
        });
    };

    jobs::RunSync(init);
}

animation::BlendSpaceSampler::BlendSpaceSampler(const ReferenceTypeDef& type) :
    PoseSampler(type),
    m_sampler1Def(BlendSpaceSamplerTypeDef::GetTypeDef().m_sampler1.GetType(), this),
    m_sampler1(PoseSamplerTypeDef::GetTypeDef(), this),
    m_sampler2Def(BlendSpaceSamplerTypeDef::GetTypeDef().m_sampler2.GetType(), this),
    m_sampler2(PoseSamplerTypeDef::GetTypeDef(), this)
{
}

animation::BlendSpaceSampler::~BlendSpaceSampler()
{
}

const math::Matrix& animation::BlendSpaceSampler::SampleTransform(const animation::Animator& animator, const geo::Skeleton& skeleton, const std::string& bone, double time)
{
    using namespace math;

    time -= m_timeOffset;

    PoseSampler* sampler1 = m_sampler1.GetValue<PoseSampler*>();
    PoseSampler* sampler2 = m_sampler2.GetValue<PoseSampler*>();

    if (m_alpha < GetFloatEPS())
    {
        return sampler1->SampleTransform(animator, skeleton, bone, time);
    }

    if (m_alpha > 1 - GetFloatEPS())
    {
        return sampler2->SampleTransform(animator, skeleton, bone, time);
    }

    Matrix m1 = sampler1->SampleTransform(animator, skeleton, bone, time);
    Matrix m2 = sampler2->SampleTransform(animator, skeleton, bone, time);

    Transform mid = Lerp(m1.ToTransform(), m2.ToTransform(), m_alpha);

    return mid.ToMatrix();
}
