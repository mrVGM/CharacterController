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
		TypeProperty m_renderPasses;

		RendererTypeDef();
		virtual ~RendererTypeDef();

		void Construct(Value& container) const override;
	};

	class RendererObj : public ObjectValue
	{
	private:
		Value m_renderPasses;
	public:
		Value m_renderPassesDefs;

		RendererObj(const ReferenceTypeDef& typeDef);

		void Load(jobs::Job* done);
	};

	void Boot();
}