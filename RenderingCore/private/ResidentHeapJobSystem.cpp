#include "ResidentHeapJobSystem.h"

#include "JobSystemDef.h"
#include "JobSystem.h"

namespace
{
	BasicObjectContainer<rendering::ResidentHeapJobSystemTypeDef> m_residentHeapJobSystem;
}

const rendering::ResidentHeapJobSystemTypeDef& rendering::ResidentHeapJobSystemTypeDef::GetTypeDef()
{
	if (!m_residentHeapJobSystem.m_object)
	{
		m_residentHeapJobSystem.m_object = new ResidentHeapJobSystemTypeDef();
	}

	return *m_residentHeapJobSystem.m_object;
}

rendering::ResidentHeapJobSystemTypeDef::ResidentHeapJobSystemTypeDef() :
	ReferenceTypeDef(&jobs::JobSystemDef::GetTypeDef(), "416F6BA7-0832-4C32-B103-C2F724362D67")
{
	m_name = "Resident Heap Job System";
	m_category = "Rendering";
}

rendering::ResidentHeapJobSystemTypeDef::~ResidentHeapJobSystemTypeDef()
{
}

void rendering::ResidentHeapJobSystemTypeDef::Construct(Value& container) const
{
	jobs::JobSystem* js = new jobs::JobSystem(*this);
	container.AssignObject(js);
}

