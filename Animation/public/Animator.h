#pragma once

#include "CompositeTypeDef.h"
#include "CompositeValue.h"

#include "MultiLoader.h"

namespace animation
{
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
	private:
		jobs::MultiLoader m_loader;

		virtual void LoadData(jobs::Job* done) override;

	public:

		Animator(const ReferenceTypeDef& type);
		virtual ~Animator();

		void Load(jobs::Job* done);
	};
}