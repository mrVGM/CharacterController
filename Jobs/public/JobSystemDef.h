#pragma once

#include "CompositeTypeDef.h"

namespace jobs
{
	class JobSystemDef : public ReferenceTypeDef
	{
		TYPE_DEF_BODY(JobSystemDef)

	public:
		TypeProperty m_numThreads;

		JobSystemDef();
		virtual ~JobSystemDef();

		void Construct(Value& container) const override;
	};
}
