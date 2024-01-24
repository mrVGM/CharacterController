#pragma once

#include "MeshActor.h"

#include "Animator.h"

#include "MultiLoader.h"

namespace game
{
	class CharacterAnimatorTypeDef : public ReferenceTypeDef
	{
		TYPE_DEF_BODY(CharacterAnimatorTypeDef)

	public:
		TypeProperty m_moveSampler;

		CharacterAnimatorTypeDef();
		virtual ~CharacterAnimatorTypeDef();

		void Construct(Value& container) const override;
	};


	class CharacterAnimator : public animation::Animator
	{
	private:
		Value m_moveSampler;
		bool m_lastTickWasFalling = false;
		double m_transitionTimeLeft = 0;

	protected:
		void LoadData(jobs::Job* done) override;

	public:
		Value m_moveSamplerDef;

		animation::PoseSampler* GetSampler();

		CharacterAnimator(const ReferenceTypeDef& type);
		virtual ~CharacterAnimator();

		virtual void Tick(double dt);
	};
}