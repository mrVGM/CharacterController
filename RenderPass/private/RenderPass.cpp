#include "RenderPass.h"

#include "ClearScreenRP.h"

#include "Material.h"
#include "DisplayTextureMaterial.h"

#include "CameraBuffer.h"

namespace
{
	BasicObjectContainer<rendering::render_pass::RenderPassTypeDef> m_renderPassTypeDef;
}

const rendering::render_pass::RenderPassTypeDef& rendering::render_pass::RenderPassTypeDef::GetTypeDef()
{
	if (!m_renderPassTypeDef.m_object)
	{
		m_renderPassTypeDef.m_object = new rendering::render_pass::RenderPassTypeDef();
	}

	return *m_renderPassTypeDef.m_object;
}

rendering::render_pass::RenderPassTypeDef::RenderPassTypeDef() :
	ReferenceTypeDef(&ReferenceTypeDef::GetTypeDef(), "F451A96F-F3AB-4C7C-B510-499716FB4617")
{
	m_name = "Generic Render Pass";
	m_category = "Render Pass";
}

rendering::render_pass::RenderPassTypeDef::~RenderPassTypeDef()
{
}

void rendering::render_pass::RenderPassTypeDef::Construct(Value& container) const
{
	throw "Can't create a pure Render Pass Object!";
}

rendering::render_pass::RenderPass::RenderPass(const ReferenceTypeDef& typeDef) :
	ObjectValue(typeDef)
{
}

rendering::render_pass::RenderPass::~RenderPass()
{
}

void rendering::render_pass::Boot()
{
	RenderPassTypeDef::GetTypeDef();
	ClearScreenRPTypeDef::GetTypeDef();
	DisplayTextureMaterialTypeDef::GetTypeDef();
	CameraBufferTypeDef::GetTypeDef();
}
