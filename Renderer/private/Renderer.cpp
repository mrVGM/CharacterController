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

#include "RenderingCore.h"

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
	m_renderPasses(ListDef::GetTypeDef(render_pass::RenderPassTypeDef::GetTypeDef()), this),
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
			DXCommandQueue* commandQueue = core::utils::GetCommandQueue();
			m_ctx.m_self->m_commandQueue.AssignObject(commandQueue);

			DXSwapChain* swapChain = core::utils::GetSwapChain();
			m_ctx.m_self->m_swapChain.AssignObject(swapChain);

			DXFence* fence = core::utils::GetRenderFence();
			m_ctx.m_self->m_renderFence.AssignObject(fence);

			ValueList* prDefs = m_ctx.m_self->m_renderPassesDefs.GetValue<ValueList*>();
			ValueList* prs = m_ctx.m_self->m_renderPasses.GetValue<ValueList*>();

			std::list<ObjectValue*> tmp;
			ObjectValueContainer& container = ObjectValueContainer::GetContainer();

			m_ctx.m_loading = 0;
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

	rendering::core::LoadCoreObjects(new LoadRPs(*ctx));
}

void rendering::renderer::RendererObj::RenderFrame()
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
	commandQueue->GetGraphicsCommandQueue()->Signal(fence->GetFence(), curFrame);

	waitFence.Wait(curFrame);

	m_swapChain.GetValue<DXSwapChain*>()->Present();
}

void rendering::renderer::Boot()
{
	RendererAppEntryTypeDef::GetTypeDef();
	RendererTypeDef::GetTypeDef();
}