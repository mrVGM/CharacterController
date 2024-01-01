#pragma once

#include "TickUpdater.h"
#include "Actor.h"

#include "Animation.h"

#include "MultiLoader.h"

namespace animation
{
	class AnimatorTypeDef : public ReferenceTypeDef
	{
		TYPE_DEF_BODY(AnimatorTypeDef)

	public:
		TypeProperty m_idle;

		AnimatorTypeDef();
		virtual ~AnimatorTypeDef();

		void Construct(Value& container) const override;
	};


	class Animator : public runtime::TickUpdater, public jobs::LoadingClass
	{
	private:
		double m_curTime = 0;
		Value m_actor;
		Value m_idle;
		
		const math::Matrix& SampleTransform(double time, const std::string& bone, const geo::Animation& animation);

	protected:
		virtual void LoadData(jobs::Job* done) override;

	public:
		Value m_idleDef;

		Animator(const ReferenceTypeDef& type);
		virtual ~Animator();

		virtual bool IsTicking() override;
		virtual void Tick(double dt, jobs::Job* done) override;

		void SetActor(runtime::Actor & actor);
	};
}