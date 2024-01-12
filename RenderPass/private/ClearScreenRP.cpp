#include "ClearScreenRP.h"

#include "DXSwapChain.h"
#include "DXCommandQueue.h"

#include "Jobs.h"

#include "DXDevice.h"
#include "DXSwapChain.h"
#include "DXCommandQueue.h"

#include "ObjectValueContainer.h"
#include "DXDescriptorHeap.h"

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

	Value deviceVal(DXDeviceTypeDef::GetTypeDef(), nullptr);
	ObjectValueContainer::GetObjectOfType(DXDeviceTypeDef::GetTypeDef(), deviceVal);
	DXDevice* device = deviceVal.GetValue<DXDevice*>();
	
	ObjectValueContainer::GetObjectOfType(DXSwapChainTypeDef::GetTypeDef(), m_swapChain);
	ObjectValueContainer::GetObjectOfType(DXCommandQueueTypeDef::GetTypeDef(), m_commandQueue);
	ObjectValueContainer::GetObjectOfType(DepthStencilDescriptorHeapTypeDef::GetTypeDef(), m_dsDescriptorHeap);

	THROW_ERROR(
		device->GetDevice().CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)),
		"Can't create Command Allocator!")

	THROW_ERROR(
		device->GetDevice().CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList0)),
		"Can't create Command List!")

	THROW_ERROR(
		m_commandList0->Close(),
		"Can't close command List!")

	THROW_ERROR(
		device->GetDevice().CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList1)),
		"Can't create Command List!")

	THROW_ERROR(
		m_commandList1->Close(),
		"Can't close command List!")
}

rendering::render_pass::ClearScreenRP::ClearScreenRP(const ReferenceTypeDef& typeDef) :
	RenderPass(typeDef),
	m_swapChain(DXSwapChainTypeDef::GetTypeDef(), this),
	m_dsDescriptorHeap(DXDescriptorHeapTypeDef::GetTypeDef(), this),
	m_commandQueue(DXCommandQueueTypeDef::GetTypeDef(), this)
{
}

rendering::render_pass::ClearScreenRP::~ClearScreenRP()
{
}

void rendering::render_pass::ClearScreenRP::Prepare()
{
	DXSwapChain* swapChain = m_swapChain.GetValue<DXSwapChain*>();
	int frameIndex = swapChain->GetCurrentSwapChainIndex();

	if (m_commnadListsRecorded[frameIndex])
	{
		return;
	}
	m_commnadListsRecorded[frameIndex] = true;

	ID3D12GraphicsCommandList* commandList = frameIndex == 0 ? m_commandList0.Get() : m_commandList1.Get();

	THROW_ERROR(
		commandList->Reset(m_commandAllocator.Get(), nullptr),
		"Can't reset Command List!")

	{
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(swapChain->GetCurrentRenderTarget(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		commandList->ResourceBarrier(1, &barrier);
	}

	const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	commandList->ClearRenderTargetView(swapChain->GetCurrentRTVDescriptor(), clearColor, 0, nullptr);

	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_dsDescriptorHeap.GetValue<DXDescriptorHeap*>()->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
	commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	{
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(swapChain->GetCurrentRenderTarget(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		commandList->ResourceBarrier(1, &barrier);
	}

	THROW_ERROR(
		commandList->Close(),
		"Can't close Command List!")
}

void rendering::render_pass::ClearScreenRP::Execute()
{
	DXCommandQueue* commandQueue = m_commandQueue.GetValue<DXCommandQueue*>();

	DXSwapChain* swapChain = m_swapChain.GetValue<DXSwapChain*>();
	int frameIndex = swapChain->GetCurrentSwapChainIndex();

	ID3D12CommandList* ppCommandLists[] = { frameIndex == 0 ? m_commandList0.Get() : m_commandList1.Get() };
	commandQueue->GetGraphicsCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
}

void rendering::render_pass::ClearScreenRP::LoadData(jobs::Job* done)
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