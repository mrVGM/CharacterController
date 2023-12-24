#include "RendererAppEntry.h"

#include "AppEntry.h"

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
	AppEntryObj(typeDef),
	m_renderer(renderer::RendererTypeDef::GetTypeDef(), this)
{
}

rendering::RendererAppEntryObj::~RendererAppEntryObj()
{
}

void rendering::RendererAppEntryObj::Boot()
{
	struct Context
	{
		RendererAppEntryObj* m_self = nullptr;
	};

	Context ctx{ this };

	class RenderFrame : public jobs::Job
	{
	private:
		Context m_ctx;

	public:
		RenderFrame(const Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			renderer::RendererObj* renderer = m_ctx.m_self->m_renderer.GetValue<renderer::RendererObj*>();
			renderer->RenderFrame();
			jobs::RunAsync(new RenderFrame(m_ctx));
		}
	};

	class RendererLoaded : public jobs::Job
	{
	private:
		Context m_ctx;

	public:
		RendererLoaded(const Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			renderer::RendererObj* renderer = m_ctx.m_self->m_renderer.GetValue<renderer::RendererObj*>();
			jobs::RunAsync(new RenderFrame(m_ctx));
		}
	};

	class LoadRenderer : public jobs::Job
	{
	private:
		Context m_ctx;

	public:
		LoadRenderer(const Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			std::list<ObjectValue*> tmp;
			ObjectValueContainer& container = ObjectValueContainer::GetContainer();
			container.GetObjectsOfType(renderer::RendererTypeDef::GetTypeDef(), tmp);

			renderer::RendererObj* renderer = static_cast<renderer::RendererObj*>(tmp.front());
			m_ctx.m_self->m_renderer.AssignObject(renderer);

			renderer->Load(new RendererLoaded(m_ctx));
		}
	};

	jobs::RunSync(new LoadRenderer(ctx));

}
