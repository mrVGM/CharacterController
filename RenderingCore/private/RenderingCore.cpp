#include "RenderingCore.h"

#include "RenderWindow.h"
#include "RenderingEntryTypeDef.h"
#include "DXShader.h"
#include "DXDevice.h"
#include "DXSwapChain.h"
#include "DXCommandQueue.h"
#include "DXFence.h"
#include "DXHeap.h"
#include "DXBuffer.h"
#include "DXCopyCommandQueue.h"

#include "ResidentHeapFence.h"
#include "ResidentHeapJobSystem.h"
#include "RenderFence.h"
#include "DXCopyBuffers.h"

#include "ObjectValueContainer.h"

#include "CoreUtils.h"

namespace
{
	void LoadStage0(jobs::Job* done)
	{
		struct Context
		{
			int m_loading = 0;
			jobs::Job* m_done = nullptr;
		};
		Context* ctx = new Context{ 0, done };

		class ItemLoaded : public jobs::Job
		{
		private:
			Context& m_ctx;
		public:
			ItemLoaded(Context& ctx) :
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

		class LoadJob : public jobs::Job
		{
		private:
			Context& m_ctx;
		public:
			LoadJob(Context& ctx) :
				m_ctx(ctx)
			{
			}

			void Do() override
			{
				using namespace rendering;

				WindowObj* window = core::utils::GetWindow();
				window->Start();

				jobs::JobSystem* residentHeapJobSystem = static_cast<jobs::JobSystem*>(ObjectValueContainer::GetObjectOfType(ResidentHeapJobSystemTypeDef::GetTypeDef()));
				residentHeapJobSystem->Start();

				DXCommandQueue* commandQueue = core::utils::GetCommandQueue();
				DXCopyCommandQueue* copyCommandQueue = core::utils::GetCopyCommandQueue();
				DXFence* renderFence = core::utils::GetRenderFence();
				DXFence* residentHeapFence = core::utils::GetResidentHeapFence();

				m_ctx.m_loading = 4;

				commandQueue->Load(new ItemLoaded(m_ctx));
				copyCommandQueue->Load(new ItemLoaded(m_ctx));
				renderFence->Load(new ItemLoaded(m_ctx));
				residentHeapFence->Load(new ItemLoaded(m_ctx));
			}
		};

		jobs::RunSync(new LoadJob(*ctx));
	}

	void LoadStage1(jobs::Job* done)
	{
		struct Context
		{
			int m_loading = 0;
			jobs::Job* m_done = nullptr;
		};
		Context* ctx = new Context{ 0, done };

		class ItemLoaded : public jobs::Job
		{
		private:
			Context& m_ctx;
		public:
			ItemLoaded(Context& ctx) :
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
				delete& m_ctx;
			}
		};

		class LoadJob : public jobs::Job
		{
		private:
			Context& m_ctx;
		public:
			LoadJob(Context& ctx) :
				m_ctx(ctx)
			{
			}

			void Do() override
			{
				using namespace rendering;

				DXSwapChain* swapChain = core::utils::GetSwapChain();
				DXCopyBuffers* copyBuffers = core::utils::GetCopyBuffers();

				m_ctx.m_loading = 2;

				swapChain->Load(new ItemLoaded(m_ctx));
				copyBuffers->Load(new ItemLoaded(m_ctx));
			}
		};

		jobs::RunSync(new LoadJob(*ctx));
	}
}

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
	DXHeapTypeDef::GetTypeDef();
	DXBufferTypeDef::GetTypeDef();
	DXCopyCommandQueueTypeDef::GetTypeDef();
	DXCopyBuffersTypeDef::GetTypeDef();

	RenderFenceTypeDef::GetTypeDef();
	ResidentHeapFenceTypeDef::GetTypeDef();
	ResidentHeapJobSystemTypeDef::GetTypeDef();
}

void rendering::core::LoadCoreObjects(jobs::Job* done)
{
	struct Context
	{
		jobs::Job* m_done = nullptr;
	};
	Context ctx{ done };

	class Stage1 : public jobs::Job
	{
	private:
		Context m_ctx;
	public:
		Stage1(const Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			LoadStage1(m_ctx.m_done);
		}
	};

	LoadStage0(new Stage1(ctx));
}