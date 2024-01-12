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

				m_ctx.m_loading = 3;

				commandQueue->Load(new ItemLoaded(m_ctx));
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

				ObjectValueContainer::GetObjectOfType(DXSwapChainTypeDef::GetTypeDef(), m_coreObjects.m_swapChain);
				ObjectValueContainer::GetObjectOfType(DXCopyBuffersTypeDef::GetTypeDef(), m_coreObjects.m_copyBuffers);

				DXSwapChain* swapChain = m_coreObjects.m_swapChain.GetValue<DXSwapChain*>();
				DXCopyBuffers* copyBuffers = m_coreObjects.m_copyBuffers.GetValue<DXCopyBuffers*>();
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