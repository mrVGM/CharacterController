#pragma once

#include "CompositeTypeDef.h"

namespace jobs
{
	class JobSystemDef : public ReferenceTypeDef
	{
	public:
		TypeProperty m_numThreads;

		static const JobSystemDef& GetJobSystemTypeDef();

		JobSystemDef();

		void Construct(Value& container) const override;
	};
}
