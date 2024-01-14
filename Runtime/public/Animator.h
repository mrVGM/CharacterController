#pragma once

#include "MeshActor.h"

#include "Animation.h"

#include "MultiLoader.h"

namespace animation
{
	class AnimatorTypeDef : public ReferenceTypeDef
	{
		TYPE_DEF_BODY(AnimatorTypeDef)

	public:
		TypeProperty m_sampler;

		AnimatorTypeDef();
		virtual ~AnimatorTypeDef();

		void Construct(Value& container) const override;
	};


	class Animator : public ObjectValue, public jobs::LoadingClass
	{
	protected:
		double m_curTime = 0;
		Value m_sampler;

		Value m_actor;

		virtual void LoadData(jobs::Job* done) override;

	public:
		Value m_samplerDef;

		Animator(const ReferenceTypeDef& type);
		virtual ~Animator();

		virtual bool IsTicking() const;
		virtual void Tick(double dt);

		void SetActor(runtime::MeshActor& actor);
	};
}