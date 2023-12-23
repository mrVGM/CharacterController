#include "ResidentHeapFence.h"

#include "DXFence.h"

namespace
{
	BasicObjectContainer<rendering::ResidentHeapFenceTypeDef> m_fence;
}

const rendering::ResidentHeapFenceTypeDef& rendering::ResidentHeapFenceTypeDef::GetTypeDef()
{
	if (!m_fence.m_object)
	{
		m_fence.m_object = new ResidentHeapFenceTypeDef();
	}

	return *m_fence.m_object;
}

rendering::ResidentHeapFenceTypeDef::ResidentHeapFenceTypeDef() :
	ReferenceTypeDef(&DXFenceTypeDef::GetTypeDef(), "9A2671ED-8FFA-490F-9FF5-7E50E3120C56")
{
	m_name = "Resident Heap Fence";
	m_category = "Rendering";
}

rendering::ResidentHeapFenceTypeDef::~ResidentHeapFenceTypeDef()
{
}

void rendering::ResidentHeapFenceTypeDef::Construct(Value& container) const
{
	DXFence* fence = new DXFence(*this);
	container.AssignObject(fence);
}

