#pragma once

#include "MeshActor.h"

#include "Animation.h"

#include "MultiLoader.h"

namespace animation
{
	class PoseSampler;

	class AnimatorTypeDef : public ReferenceTypeDef
	{
		TYPE_DEF_BODY(AnimatorTypeDef)

	public:
		AnimatorTypeDef();
		virtual ~AnimatorTypeDef();

		void Construct(Value& container) const override;
	};


	class Animator : public ObjectValue, public jobs::LoadingClass
	{
	protected:
		double m_curTime = 0;
		Value m_actor;

		virtual void LoadData(jobs::Job* done) override;

	public:
		Animator(const ReferenceTypeDef& type);
		virtual ~Animator();

		virtual PoseSampler* GetSampler();

		virtual bool IsTicking() const;
		virtual void Tick(double dt);

		void SetActor(runtime::MeshActor& actor);
	};
}