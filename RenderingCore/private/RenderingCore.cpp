#include "RenderingCore.h"

#include "ListDef.h"

#include "RenderWindow.h"
#include "DXShader.h"
#include "DXDevice.h"
#include "DXSwapChain.h"
#include "DXCommandQueue.h"
#include "DXFence.h"
#include "DXHeap.h"
#include "DXBuffer.h"
#include "DXTexture.h"
#include "DXDepthStencilTexture.h"
#include "DXDescriptorHeap.h"

#include "ResidentHeapFence.h"
#include "ResidentHeapJobSystem.h"
#include "RenderFence.h"
#include "DXCopyBuffers.h"
#include "DXMutableBuffer.h"

#include "RenderWindow.h"
#include "RenderFence.h"

#include "ObjectValueContainer.h"

namespace
{
	struct CoreObjects
	{
		Value m_window;
		Value m_device;
		Value m_commandQueue;
		Value m_residentHeapJS;
		Value m_renderFence;
		Value m_residentHeapFence;
		Value m_swapChain;
		Value m_copyBuffers;

		CoreObjects() :
			m_window(rendering::WindowTypeDef::GetTypeDef(), nullptr),
			m_device(rendering::DXDeviceTypeDef::GetTypeDef(), nullptr),
			m_commandQueue(rendering::DXCommandQueueTypeDef::GetTypeDef(), nullptr),
			m_residentHeapJS(rendering::ResidentHeapJobSystemTypeDef::GetTypeDef(), nullptr),
			m_renderFence(rendering::DXFenceTypeDef::GetTypeDef(), nullptr),
			m_residentHeapFence(rendering::DXFenceTypeDef::GetTypeDef(), nullptr),
			m_swapChain(rendering::DXSwapChainTypeDef::GetTypeDef(), nullptr),
			m_copyBuffers(rendering::DXCopyBuffersTypeDef::GetTypeDef(), nullptr)
		{
		}
	};

	CoreObjects m_coreObjects;

	void LoadStage0(jobs::Job done)
	{
		struct Context
		{
			int m_loading = 0;
		};
		Context* ctx = new Context{ 0 };

		jobs::Job itemLoaded = [=]() {
			--ctx->m_loading;
			if (ctx->m_loading > 0)
			{
				return;
			}
			delete ctx;

			jobs::RunSync(done);
		};

		jobs::Job loadJob = [=]() {
			using namespace rendering;

			ObjectValueContainer::GetObjectOfType(WindowTypeDef::GetTypeDef(), m_coreObjects.m_window);
			ObjectValueContainer::GetObjectOfType(DXDeviceTypeDef::GetTypeDef(), m_coreObjects.m_device);
			ObjectValueContainer::GetObjectOfType(DXCommandQueueTypeDef::GetTypeDef(), m_coreObjects.m_commandQueue);
			ObjectValueContainer::GetObjectOfType(ResidentHeapJobSystemTypeDef::GetTypeDef(), m_coreObjects.m_residentHeapJS);
			ObjectValueContainer::GetObjectOfType(ResidentHeapFenceTypeDef::GetTypeDef(), m_coreObjects.m_residentHeapFence);
			ObjectValueContainer::GetObjectOfType(RenderFenceTypeDef::GetTypeDef(), m_coreObjects.m_renderFence);

			WindowObj* window = m_coreObjects.m_window.GetValue<WindowObj*>();
			window->Start();

			jobs::JobSystem* residentHeapJobSystem = m_coreObjects.m_residentHeapJS.GetValue<jobs::JobSystem*>();
			residentHeapJobSystem->Start();

			DXCommandQueue* commandQueue = m_coreObjects.m_commandQueue.GetValue<DXCommandQueue*>();
			DXFence* renderFence = m_coreObjects.m_renderFence.GetValue<DXFence*>();
			DXFence* residentHeapFence = m_coreObjects.m_residentHeapFence.GetValue<DXFence*>();

			ctx->m_loading = 3;

			commandQueue->Load(itemLoaded);
			renderFence->Load(itemLoaded);
			residentHeapFence->Load(itemLoaded);
		};

		jobs::RunSync(loadJob);
	}

	void LoadStage1(jobs::Job done)
	{
		struct Context
		{
			int m_loading = 0;
		};
		Context* ctx = new Context{ 0 };

		jobs::Job itemLoaded = [=]() {
			--ctx->m_loading;
			if (ctx->m_loading > 0)
			{
				return;
			}
			delete ctx;

			jobs::RunSync(done);
		};

		jobs::Job loadJob = [=]() {
			using namespace rendering;

			ObjectValueContainer::GetObjectOfType(DXSwapChainTypeDef::GetTypeDef(), m_coreObjects.m_swapChain);
			ObjectValueContainer::GetObjectOfType(DXCopyBuffersTypeDef::GetTypeDef(), m_coreObjects.m_copyBuffers);

			DXSwapChain* swapChain = m_coreObjects.m_swapChain.GetValue<DXSwapChain*>();
			DXCopyBuffers* copyBuffers = m_coreObjects.m_copyBuffers.GetValue<DXCopyBuffers*>();
			ctx->m_loading = 2;

			swapChain->Load(itemLoaded);
			copyBuffers->Load(itemLoaded);
		};

		jobs::RunSync(loadJob);
	}
}

void rendering::core::Boot()
{
	WindowTypeDef::GetTypeDef();

	DXShaderTypeDef::GetTypeDef();
	DXVertexShaderTypeDef::GetTypeDef();
	DXPixelShaderTypeDef::GetTypeDef();

	DXDeviceTypeDef::GetTypeDef();
	DXSwapChainTypeDef::GetTypeDef();
	DXCommandQueueTypeDef::GetTypeDef();
	DXFenceTypeDef::GetTypeDef();
	DXHeapTypeDef::GetTypeDef();
	DXBufferTypeDef::GetTypeDef();
	DXCopyBuffersTypeDef::GetTypeDef();
	DXMutableBufferTypeDef::GetTypeDef();
	DXTextureTypeDef::GetTypeDef();
	DXDepthStencilTextureTypeDef::GetTypeDef();
	DXDescriptorHeapTypeDef::GetTypeDef();
	DepthStencilDescriptorHeapTypeDef::GetTypeDef();
	RenderTargetDescriptorHeapTypeDef::GetTypeDef();
	ShaderResourceDescriptorHeapTypeDef::GetTypeDef();

	RenderFenceTypeDef::GetTypeDef();
	ResidentHeapFenceTypeDef::GetTypeDef();
	ResidentHeapJobSystemTypeDef::GetTypeDef();
}

void rendering::core::LoadCoreObjects(jobs::Job done)
{
	LoadStage0([=]() {
		LoadStage1(done);
	});
}