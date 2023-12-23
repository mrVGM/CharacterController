#include "RenderingCore.h"

#include "RenderWindow.h"
#include "RenderingEntryTypeDef.h"
#include "DXShader.h"
#include "DXDevice.h"
#include "DXSwapChain.h"
#include "DXCommandQueue.h"
#include "DXFence.h"

#include "ResidentHeapFence.h"
#include "ResidentHeapJobSystem.h"
#include "RenderFence.h"

#include "ObjectValueContainer.h"

#include "CoreUtils.h"

void rendering::core::Boot()
{
	WindowTypeDef::GetTypeDef();
	RenderingEntryTypeDef::GetTypeDef();

	DXShaderTypeDef::GetTypeDef();
	DXVertexShaderTypeDef::GetTypeDef();
	DXPixelShaderTypeDef::GetTypeDef();

	DXDeviceTypeDef::GetTypeDef();
	DXSwapChainTypeDef::GetTypeDef();
	DXCommandQueueTypeDef::GetTypeDef();
	DXFenceTypeDef::GetTypeDef();

	RenderFenceTypeDef::GetTypeDef();
	ResidentHeapFenceTypeDef::GetTypeDef();
	ResidentHeapJobSystemTypeDef::GetTypeDef();
}

void rendering::core::LoadCoreObjects(jobs::Job* done)
{
	struct Context
	{
		int m_loading = 0;
		jobs::Job* m_done = nullptr;
	};

	Context* ctx = new Context{ 0, done };

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
			swapChain->Load(m_ctx.m_done);

			delete &m_ctx;
		}
	};

	class InitialItemLoaded : public jobs::Job
	{
	private:
		Context& m_ctx;
	public:
		InitialItemLoaded(Context& ctx) :
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

			jobs::RunSync(new LoadSwapChain(m_ctx));
		}
	};

	class LoadInitialObjects : public jobs::Job
	{
	private:
		Context& m_ctx;
	public:
		LoadInitialObjects(Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			WindowObj* window = core::utils::GetWindow();
			window->Start();

			jobs::JobSystem* residentHeapJobSystem = static_cast<jobs::JobSystem*>(ObjectValueContainer::GetObjectOfType(ResidentHeapJobSystemTypeDef::GetTypeDef()));
			residentHeapJobSystem->Start();

			DXCommandQueue* commandQueue = core::utils::GetCommandQueue();
			DXFence* renderFence = core::utils::GetRenderFence();
			DXFence* residentHeapFence = core::utils::GetResidentHeapFence();

			m_ctx.m_loading = 3;

			commandQueue->Load(new InitialItemLoaded(m_ctx));
			renderFence->Load(new InitialItemLoaded(m_ctx));
			residentHeapFence->Load(new InitialItemLoaded(m_ctx));
		}
	};

	jobs::RunSync(new LoadInitialObjects(*ctx));
}