#pragma once

#include "CompositeTypeDef.h"
#include "CompositeValue.h"

#include "Job.h"

namespace rendering::renderer
{
	class RendererTypeDef : public ReferenceTypeDef
	{
		TYPE_DEF_BODY(RendererTypeDef)

	public:
		TypeProperty m_renderFence;
		TypeProperty m_renderPasses;

		RendererTypeDef();
		virtual ~RendererTypeDef();

		void Construct(Value& container) const override;
	};

	class RendererObj : public ObjectValue
	{
	private:
		int m_frameIndex = 1;
		Value m_renderPasses;

		Value m_swapChain;
		Value m_commandQueue;
		Value m_renderFence;
	public:
		Value m_renderFenceDef;
		Value m_renderPassesDefs;

		RendererObj(const ReferenceTypeDef& typeDef);

		void Load(jobs::Job* done);

		void RenderFrame();
	};

	void Boot();
}