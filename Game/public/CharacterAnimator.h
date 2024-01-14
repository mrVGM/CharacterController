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
		CharacterAnimatorTypeDef();
		virtual ~CharacterAnimatorTypeDef();

		void Construct(Value& container) const override;
	};


	class CharacterAnimator : public animation::Animator
	{
	public:
		CharacterAnimator(const ReferenceTypeDef& type);
		virtual ~CharacterAnimator();

		virtual void Tick(double dt);
	};
}