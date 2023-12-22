#include "Renderer.h"

#include "PrimitiveTypes.h"
#include "ListDef.h"
#include "DXFence.h"

#include "RendererAppEntry.h"
#include "RenderPass.h"

#include "Jobs.h"
#include "ValueList.h"
#include "ObjectValueContainer.h"

#include "WaitFence.h"

#include "CoreUtils.h"

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
		ListDef::GetTypeDef(TypeTypeDef::GetTypeDef(render_pass::RenderPassTypeDef::GetTypeDef()))),
	m_renderFence(
		"27D603E6-8128-4546-8AE0-28459ED1553A",
		TypeTypeDef::GetTypeDef(DXFenceTypeDef::GetTypeDef()))
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

	{
		m_renderFence.m_name = "Render Fence";
		m_renderFence.m_category = "Setup";
		m_renderFence.m_getValue = [](CompositeValue* value) -> Value& {
			RendererObj* rendererObj = static_cast<RendererObj*>(value);
			return rendererObj->m_renderFenceDef;
		};

		m_properties[m_renderFence.GetId()] = &m_renderFence;
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
	m_renderPasses(ListDef::GetTypeDef(render_pass::RenderPassTypeDef::GetTypeDef()), this),
	m_renderFenceDef(RendererTypeDef::GetTypeDef().m_renderFence.GetType(), this),
	m_renderFence(DXFenceTypeDef::GetTypeDef(), this),
	m_swapChain(DXSwapChainTypeDef::GetTypeDef(), this),
	m_commandQueue(DXCommandQueueTypeDef::GetTypeDef(), this)
{
}

void rendering::renderer::RendererObj::Load(jobs::Job* done)
{
	struct Context
	{
		int m_loading = 0;
		RendererObj* m_self = nullptr;
		jobs::Job* m_done;
	};

	Context* ctx = new Context{ 0, this, done };

	class RPLoaded : public jobs::Job
	{
	private:
		Context& m_ctx;
	public:
		RPLoaded(Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			--m_ctx.m_loading;

			if (m_ctx.m_loading > 0)
			{
				return;
			}

			jobs::RunSync(m_ctx.m_done);
			delete &m_ctx;
		}
	};

	class LoadRPs : public jobs::Job
	{
	private:
		Context& m_ctx;
	public:
		LoadRPs(Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			std::list<ObjectValue*> tmp;
			ObjectValueContainer& container = ObjectValueContainer::GetContainer();

			container.GetObjectsOfType(*m_ctx.m_self->m_renderFenceDef.GetType<const TypeDef*>(), tmp);
			DXFence* fence = static_cast<DXFence*>(tmp.front());
			m_ctx.m_self->m_renderFence.AssignObject(fence);

			m_ctx.m_loading = 1;
			fence->Load(new RPLoaded(m_ctx));

			ValueList* prDefs = m_ctx.m_self->m_renderPassesDefs.GetValue<ValueList*>();
			ValueList* prs = m_ctx.m_self->m_renderPasses.GetValue<ValueList*>();

			for (auto it = prDefs->GetIterator(); it; ++it)
			{
				const render_pass::RenderPassTypeDef* curRPDef = (*it).GetType<const render_pass::RenderPassTypeDef*>();
				tmp.clear();
				container.GetObjectsOfType(*curRPDef, tmp);

				render_pass::RenderPass* rp = static_cast<render_pass::RenderPass*>(tmp.front());
				Value& val = prs->EmplaceBack();
				val.AssignObject(rp);
				++m_ctx.m_loading;

				rp->Load(new RPLoaded(m_ctx));
			}
		}
	};

	class LoadSwapChain : public jobs::Job
	{
	private:
		Context& m_ctx;
	public:
		LoadSwapChain(Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			DXSwapChain* swapChain = core::utils::GetSwapChain();
			m_ctx.m_self->m_swapChain.AssignObject(swapChain);
			swapChain->Load(new LoadRPs(m_ctx));
		}
	};

	class LoadCommandQueue : public jobs::Job
	{
	private:
		Context& m_ctx;
	public:
		LoadCommandQueue(Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			WindowObj* window = core::utils::GetWindow();
			window->Start();

			DXCommandQueue* commandQueue = core::utils::GetCommandQueue();

			m_ctx.m_self->m_commandQueue.AssignObject(commandQueue);
			commandQueue->Load(new LoadSwapChain(m_ctx));
		}
	};

	jobs::RunSync(new LoadCommandQueue(*ctx));
}

void rendering::renderer::RendererObj::RenderFrame(jobs::Job* done)
{
	ValueList* passes = m_renderPasses.GetValue<ValueList*>();

	for (auto it = passes->GetIterator(); it; ++it)
	{
		render_pass::RenderPass* rp = (*it).GetValue<render_pass::RenderPass*>();
		rp->Prepare();
	}

	for (auto it = passes->GetIterator(); it; ++it)
	{
		render_pass::RenderPass* rp = (*it).GetValue<render_pass::RenderPass*>();
		rp->Execute();
	}

	DXFence* fence = m_renderFence.GetValue<DXFence*>();
	int curFrame = m_frameIndex++;

	WaitFence waitFence(*fence);

	DXCommandQueue* commandQueue = m_commandQueue.GetValue<DXCommandQueue*>();
	commandQueue->GetCommandQueue()->Signal(fence->GetFence(), curFrame);

	waitFence.Wait(curFrame);
	m_swapChain.GetValue<DXSwapChain*>()->Present();

	jobs::RunSync(done);
}

void rendering::renderer::Boot()
{
	RendererAppEntryTypeDef::GetTypeDef();
	RendererTypeDef::GetTypeDef();
}