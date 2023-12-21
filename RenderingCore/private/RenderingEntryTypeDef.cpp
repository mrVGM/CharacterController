#include "RenderingEntryTypeDef.h"

#include "RenderingEntryObj.h"

namespace
{
	BasicObjectContainer<rendering::core::RenderingEntryTypeDef> m_renderingEntryTypeDef;
}

const rendering::core::RenderingEntryTypeDef& rendering::core::RenderingEntryTypeDef::GetTypeDef()
{
	if (!m_renderingEntryTypeDef.m_object)
	{
		m_renderingEntryTypeDef.m_object = new RenderingEntryTypeDef();
	}

	return *m_renderingEntryTypeDef.m_object;
}

rendering::core::RenderingEntryTypeDef::RenderingEntryTypeDef() :
	ReferenceTypeDef(&app::AppEntryTypeDef::GetTypeDef(), "E4418686-5568-45E3-8188-D059BF5FB69B")
{
	m_name = "Rendering App Entry";
	m_category = "Rendering";
}

void rendering::core::RenderingEntryTypeDef::Construct(Value& container) const
{
	RenderingEntryObj* obj = new RenderingEntryObj(*this);
	container.AssignObject(obj);
}
