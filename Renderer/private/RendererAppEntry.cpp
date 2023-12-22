#include "RendererAppEntry.h"

#include "AppEntryTypeDef.h"

namespace
{
	BasicObjectContainer<rendering::RendererAppEntryTypeDef> m_rendererAppEntry;
}

const rendering::RendererAppEntryTypeDef& rendering::RendererAppEntryTypeDef::GetTypeDef()
{
	if (!m_rendererAppEntry.m_object)
	{
		m_rendererAppEntry.m_object = new rendering::RendererAppEntryTypeDef();
	}

	return *m_rendererAppEntry.m_object;
}

rendering::RendererAppEntryTypeDef::RendererAppEntryTypeDef() :
	ReferenceTypeDef(&app::AppEntryTypeDef::GetTypeDef(), "62CBB02A-9574-4F11-B468-F14334EC6B16")
{
	m_name = "Renderer App Entry";
	m_category = "Renderer";
}

rendering::RendererAppEntryTypeDef::~RendererAppEntryTypeDef()
{
}

void rendering::RendererAppEntryTypeDef::Construct(Value& container) const
{
	RendererAppEntryObj* entry = new RendererAppEntryObj(*this);
	container.AssignObject(entry);
}

rendering::RendererAppEntryObj::RendererAppEntryObj(const ReferenceTypeDef& typeDef) :
	AppEntryObj(typeDef)
{
}

void rendering::RendererAppEntryObj::Boot()
{
}
