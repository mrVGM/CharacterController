#pragma once

#include "Animator.h"
#include "Skeleton.h"

#include "MultiLoader.h"

namespace animation
{
	class PoseSamplerTypeDef : public ReferenceTypeDef
	{
		TYPE_DEF_BODY(PoseSamplerTypeDef)

	public:
		PoseSamplerTypeDef();
		virtual ~PoseSamplerTypeDef();

		void Construct(Value& container) const override;
	};


	class PoseSampler : public ObjectValue, public jobs::LoadingClass
	{
	public:
		PoseSampler(const ReferenceTypeDef& type);
		virtual ~PoseSampler();

		virtual const math::Matrix& SampleTransform(
			const animation::Animator& animator,
			const geo::Skeleton& skeleton,
			const std::string& bone,
			double time
		) = 0;
	};
}