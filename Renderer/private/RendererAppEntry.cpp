#include "RendererAppEntry.h"

#include "AppEntryTypeDef.h"

#include "Jobs.h"
#include "ObjectValueContainer.h"
#include "Renderer.h"


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

rendering::RendererAppEntryObj::~RendererAppEntryObj()
{
}

void rendering::RendererAppEntryObj::Boot()
{
	class RendererLoaded : public jobs::Job
	{
	public:
		void Do() override
		{
		}
	};

	class LoadRenderer : public jobs::Job
	{
	public:
		void Do() override
		{
			std::list<ObjectValue*> tmp;
			ObjectValueContainer& container = ObjectValueContainer::GetContainer();
			container.GetObjectsOfType(renderer::RendererTypeDef::GetTypeDef(), tmp);

			renderer::RendererObj* renderer = static_cast<renderer::RendererObj*>(tmp.front());
			renderer->Load(new RendererLoaded());
		}
	};

	jobs::RunSync(new LoadRenderer());

}
