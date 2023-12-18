#include "JobSystemDef.h"

#include "JobSystem.h"

#include "JobSystem.h"
#include "PrimitiveTypes.h"

namespace
{
	BasicObjectContainer<jobs::JobSystemDef> m_jobSystemDef;
}

const jobs::JobSystemDef& jobs::JobSystemDef::GetJobSystemTypeDef()
{
	if (!m_jobSystemDef.m_object)
	{
		m_jobSystemDef.m_object = new JobSystemDef();
	}

	return *m_jobSystemDef.m_object;
}

jobs::JobSystemDef::JobSystemDef() :
	ReferenceTypeDef(nullptr, "061DFEE1-B9C6-406D-BFEC-E89830C7FD3D"),
	m_numThreads("C519F2FA-A756-42E1-B362-15556A2758D8", IntTypeDef::GetTypeDef())
{
	{
		m_numThreads.m_name = "Num Threads";
		m_numThreads.m_category = "Setup";

		m_numThreads.m_getValue = [](CompositeValue* object) -> Value& {
			JobSystem* js = static_cast<JobSystem*>(object);
			return js->m_numThreads;
		};

		m_properties[m_numThreads.GetId()] = &m_numThreads;
	}

	m_name = "Job System";
	m_category = "Jobs";
}

void jobs::JobSystemDef::Construct(Value& container) const
{
	JobSystem* js = new JobSystem(*this, nullptr);
	container.AssignObject(js);
}
