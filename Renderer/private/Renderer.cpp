#include "Renderer.h"

#include "PrimitiveTypes.h"
#include "ListDef.h"
#include "RenderPass.h"

#include "RendererAppEntry.h"

namespace
{
	BasicObjectContainer<rendering::renderer::RendererTypeDef> m_renderer;
}

const rendering::renderer::RendererTypeDef& rendering::renderer::RendererTypeDef::GetTypeDef()
{
	if (!m_renderer.m_object)
	{
		m_renderer.m_object = new RendererTypeDef();
	}

	return *m_renderer.m_object;
}

rendering::renderer::RendererTypeDef::RendererTypeDef() :
	ReferenceTypeDef(&ReferenceTypeDef::GetTypeDef(), "81C3A235-D3B7-4C3A-B921-EF1C6798E1D3"),
	m_renderPasses(
		"DA9E00D1-A9AD-4B23-9DD9-F66F7BF140CA",
		ListDef::GetTypeDef(TypeTypeDef::GetTypeDef(render_pass::RenderPassTypeDef::GetTypeDef())))
{
	{
		m_renderPasses.m_name = "Render Passes";
		m_renderPasses.m_category = "Setup";
		m_renderPasses.m_getValue = [](CompositeValue* value) -> Value& {
			RendererObj* rendererObj = static_cast<RendererObj*>(value);
			return rendererObj->m_renderPassesDefs;
		};

		m_properties[m_renderPasses.GetId()] = &m_renderPasses;
	}

	m_name = "Base Renderer";
	m_category = "Renderer";
}

rendering::renderer::RendererTypeDef::~RendererTypeDef()
{
}

void rendering::renderer::RendererTypeDef::Construct(Value& container) const
{
	RendererObj* rendererObj = new RendererObj(*this);
	container.AssignObject(rendererObj);
}

rendering::renderer::RendererObj::RendererObj(const ReferenceTypeDef& typeDef) :
	ObjectValue(typeDef),
	m_renderPassesDefs(RendererTypeDef::GetTypeDef().m_renderPasses.GetType(), this),
	m_renderPasses(ListDef::GetTypeDef(render_pass::RenderPassTypeDef::GetTypeDef()), this)
{
}

void rendering::renderer::RendererObj::Load(jobs::Job* done)
{
	throw "Not implemented!";
}

void rendering::renderer::Boot()
{
	RendererAppEntryTypeDef::GetTypeDef();
	RendererTypeDef::GetTypeDef();
}