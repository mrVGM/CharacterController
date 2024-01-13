#include "PoseSampler.h"

namespace
{
    BasicObjectContainer<animation::PoseSamplerTypeDef> m_poseSamplerTypeDef;
}

const animation::PoseSamplerTypeDef& animation::PoseSamplerTypeDef::GetTypeDef()
{
    if (!m_poseSamplerTypeDef.m_object)
    {
        m_poseSamplerTypeDef.m_object = new PoseSamplerTypeDef();
    }

    return *m_poseSamplerTypeDef.m_object;
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

