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

#include "SceneObject.h"
#include "Material.h"

#include "TickUpdater.h"

#include "DXSwapChain.h"
#include "DXCommandQueue.h"
#include "DXFence.h"
#include "RenderFence.h"

#include "Camera.h"

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
	m_commandQueue(DXCommandQueueTypeDef::GetTypeDef(), this),
	m_scene(scene::SceneObjectTypeDef::GetTypeDef(), this),
	m_camera(CameraTypeDef::GetTypeDef(), this)
{
}

rendering::renderer::RendererObj::~RendererObj()
{
}

void rendering::renderer::RendererObj::Load(jobs::Job done)
{
	struct Context
	{
		int m_loading = 0;
	};

	Context* ctx = new Context();


	auto rpLoaded = [=]() {
		--ctx->m_loading;
		if (ctx->m_loading > 0)
		{
			return;
		}

		delete ctx;

		jobs::RunSync(done);
	};

	auto loadRP = [=](render_pass::RenderPass* rp) {
		return [=]() {
			rp->Load(rpLoaded);
		};
	};

	jobs::Job loadRPs = [=]() {
		ValueList* prDefs = m_renderPassesDefs.GetValue<ValueList*>();
		ValueList* prs = m_renderPasses.GetValue<ValueList*>();

		for (auto it = prDefs->GetIterator(); it; ++it)
		{
			const render_pass::RenderPassTypeDef* curRPDef = (*it).GetType<const render_pass::RenderPassTypeDef*>();
			Value& val = prs->EmplaceBack();
			ObjectValueContainer::GetObjectOfType(*curRPDef, val);
			render_pass::RenderPass* rp = val.GetValue<render_pass::RenderPass*>();

			++ctx->m_loading;

			jobs::RunAsync(loadRP(rp));
		}
	};

	jobs::Job loadScene = [=]() {
		
		scene::SceneObject* mainScene = m_scene.GetValue<scene::SceneObject*>();

		jobs::Job loadJob = [=]() {
			mainScene->Load(loadRPs);
		};

		jobs::RunAsync(loadJob);
	};

	jobs::Job loadCamera = [=]() {
		Camera* cam = m_camera.GetValue<Camera*>();

		jobs::RunAsync([=]() {
			cam->Load(loadScene);
		});
	};

	jobs::Job init = [=]() {
		ObjectValueContainer::GetObjectOfType(DXCommandQueueTypeDef::GetTypeDef(), m_commandQueue);
		ObjectValueContainer::GetObjectOfType(DXSwapChainTypeDef::GetTypeDef(), m_swapChain);		
		ObjectValueContainer::GetObjectOfType(RenderFenceTypeDef::GetTypeDef(), m_renderFence);

		ObjectValueContainer::GetObjectOfType(scene::SceneObjectTypeDef::GetTypeDef(), m_scene);
		ObjectValueContainer::GetObjectOfType(CameraTypeDef::GetTypeDef(), m_camera);

		jobs::RunSync(loadCamera);
	};

	jobs::RunAsync([=]() {
		core::LoadCoreObjects(init);
	});
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
	UINT64 curFrame = m_frameIndex++;

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
	CameraTypeDef::GetTypeDef();
}

Value& rendering::renderer::RendererObj::GetScene()
{
	return m_scene;
}