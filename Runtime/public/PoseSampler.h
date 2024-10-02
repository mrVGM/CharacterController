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
		double m_timeOffset = 0;

		PoseSampler(const ReferenceTypeDef& type);
		virtual ~PoseSampler();

		virtual const math::Matrix& SampleTransform(
			const animation::Animator& animator,
			const geo::Skeleton& skeleton,
			const std::string& bone,
			double time
		) = 0;
	};


	class AnimationSamplerTypeDef : public ReferenceTypeDef
	{
		TYPE_DEF_BODY(AnimationSamplerTypeDef)

	public:
		TypeProperty m_animation;

		AnimationSamplerTypeDef();
		virtual ~AnimationSamplerTypeDef();

		void Construct(Value& container) const override;
	};

	class AnimationSampler : public PoseSampler
	{
	private:
		Value m_animation;

		math::Matrix SampleAnimation(double time, const std::string& bone, const geo::Skeleton& skeleton);

	protected:
		void LoadData(jobs::Job done) override;

	public:
		Value m_animationDef;

		AnimationSampler(const ReferenceTypeDef& type);
		virtual ~AnimationSampler();

		virtual const math::Matrix& SampleTransform(
			const animation::Animator& animator,
			const geo::Skeleton& skeleton,
			const std::string& bone,
			double time
		) override;
	};


	class BlendSpaceSamplerTypeDef : public ReferenceTypeDef
	{
		TYPE_DEF_BODY(BlendSpaceSamplerTypeDef)

	public:
		TypeProperty m_sampler1;
		TypeProperty m_sampler2;

		BlendSpaceSamplerTypeDef();
		virtual ~BlendSpaceSamplerTypeDef();

		void Construct(Value& container) const override;
	};

	class BlendSpaceSampler : public PoseSampler
	{
	protected:
		void LoadData(jobs::Job done) override;

	public:
		Value m_sampler1;
		Value m_sampler2;

		Value m_sampler1Def;
		Value m_sampler2Def;

		float m_alpha = 0;

		BlendSpaceSampler(const ReferenceTypeDef& type);
		virtual ~BlendSpaceSampler();

		virtual const math::Matrix& SampleTransform(
			const animation::Animator& animator,
			const geo::Skeleton& skeleton,
			const std::string& bone,
			double time
		) override;
	};
}