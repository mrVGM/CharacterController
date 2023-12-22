#include "ClearScreenRP.h"

#include "DXSwapChain.h"
#include "DXCommandQueue.h"

#include "Jobs.h"

#include "CoreUtils.h"

namespace
{
	BasicObjectContainer<rendering::render_pass::ClearScreenRPTypeDef> m_clearScreenRP;
}


#define THROW_ERROR(hRes, error) \
if (FAILED(hRes)) {\
    throw error;\
}

const rendering::render_pass::ClearScreenRPTypeDef& rendering::render_pass::ClearScreenRPTypeDef::GetTypeDef()
{
	if (!m_clearScreenRP.m_object)
	{
		m_clearScreenRP.m_object = new rendering::render_pass::ClearScreenRPTypeDef();
	}

	return *m_clearScreenRP.m_object;
}

rendering::render_pass::ClearScreenRPTypeDef::ClearScreenRPTypeDef() :
	ReferenceTypeDef(&RenderPassTypeDef::GetTypeDef(), "A997F784-402E-459E-8190-370732FA2870")
{
	m_name = "Clear Screen RP";
	m_category = "Render Pass";
}

rendering::render_pass::ClearScreenRPTypeDef::~ClearScreenRPTypeDef()
{
}

void rendering::render_pass::ClearScreenRPTypeDef::Construct(Value& container) const
{
	ClearScreenRP* clearScreen = new ClearScreenRP(*this);
	container.AssignObject(clearScreen);
}

void rendering::render_pass::ClearScreenRP::Create()
{
	using Microsoft::WRL::ComPtr;

	DXDevice* device = rendering::core::utils::GetDevice();

	DXSwapChain* swapChain = rendering::core::utils::GetSwapChain();
	DXCommandQueue* commandQueue = rendering::core::utils::GetCommandQueue();

	m_swapChain.AssignObject(swapChain);
	m_commandQueue.AssignObject(commandQueue);

	THROW_ERROR(
		device->GetDevice().CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)),
		"Can't create Command Allocator!")

	THROW_ERROR(
		device->GetDevice().CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList)),
		"Can't create Command List!")

	THROW_ERROR(
		m_commandList->Close(),
		"Can't close command List!")
}

rendering::render_pass::ClearScreenRP::ClearScreenRP(const ReferenceTypeDef& typeDef) :
	RenderPass(typeDef),
	m_swapChain(DXSwapChainTypeDef::GetTypeDef(), this),
	m_commandQueue(DXCommandQueueTypeDef::GetTypeDef(), this)
{
}

rendering::render_pass::ClearScreenRP::~ClearScreenRP()
{
}

void rendering::render_pass::ClearScreenRP::Prepare()
{
	DXSwapChain* swapChain = m_swapChain.GetValue<DXSwapChain*>();

	THROW_ERROR(
		m_commandAllocator->Reset(),
		"Can't reset Command Allocator!")

	THROW_ERROR(
		m_commandList->Reset(m_commandAllocator.Get(), nullptr),
		"Can't reset Command List!")

	{
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(swapChain->GetCurrentRenderTarget(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		m_commandList->ResourceBarrier(1, &barrier);
	}

	const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	m_commandList->ClearRenderTargetView(swapChain->GetCurrentRTVDescriptor(), clearColor, 0, nullptr);

	{
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(swapChain->GetCurrentRenderTarget(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		m_commandList->ResourceBarrier(1, &barrier);
	}

	THROW_ERROR(
		m_commandList->Close(),
		"Can't close Command List!")
}

void rendering::render_pass::ClearScreenRP::Execute()
{
	DXCommandQueue* commandQueue = m_commandQueue.GetValue<DXCommandQueue*>();

	ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
	commandQueue->GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
}

void rendering::render_pass::ClearScreenRP::Load(jobs::Job* done)
{
	struct Context
	{
		ClearScreenRP* m_self = nullptr;
		jobs::Job* m_done = nullptr;
	};

	Context ctx{ this, done };

	class LoadJob : public jobs::Job
	{
	private:
		Context m_ctx;

	public:
		LoadJob(const Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			m_ctx.m_self->Create();
			jobs::RunSync(m_ctx.m_done);
		}
	};

	jobs::RunSync(new LoadJob(ctx));
}


#undef THROW_ERROR