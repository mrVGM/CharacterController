#include "RenderFence.h"

#include "DXFence.h"

namespace
{
	BasicObjectContainer<rendering::RenderFenceTypeDef> m_fence;
}

const rendering::RenderFenceTypeDef& rendering::RenderFenceTypeDef::GetTypeDef()
{
	if (!m_fence.m_object)
	{
		m_fence.m_object = new RenderFenceTypeDef();
	}

	return *m_fence.m_object;
}

rendering::RenderFenceTypeDef::RenderFenceTypeDef() :
	ReferenceTypeDef(&DXFenceTypeDef::GetTypeDef(), "C47A5CE7-86CB-408F-8322-B03E2CD1097C")
{
	m_name = "Render Fence";
	m_category = "Rendering";
}

rendering::RenderFenceTypeDef::~RenderFenceTypeDef()
{
}

void rendering::RenderFenceTypeDef::Construct(Value& container) const
{
	DXFence* fence = new DXFence(*this);
	container.AssignObject(fence);
}

