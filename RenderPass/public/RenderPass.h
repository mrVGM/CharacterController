#pragma once

#include "CompositeTypeDef.h"
#include "CompositeValue.h"

#include "MultiLoader.h"

#include "Job.h"

namespace rendering::render_pass
{
	class RenderPassTypeDef : public ReferenceTypeDef
	{
		TYPE_DEF_BODY(RenderPassTypeDef)

	public:
		RenderPassTypeDef();
		virtual ~RenderPassTypeDef();

		void Construct(Value& container) const override;
	};

	class RenderPass : public ObjectValue, public jobs::LoadingClass
	{
	protected:
		virtual void LoadData(jobs::Job* done) = 0;

	public:
		virtual void Prepare() = 0;
		virtual void Execute() = 0;

		RenderPass(const ReferenceTypeDef& typeDef);
		virtual ~RenderPass();
	};

	void Boot();
}