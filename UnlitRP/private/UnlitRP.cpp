#include "UnlitRP.h"

#include "Jobs.h"

#include "UnlitMaterial.h"

#include "ObjectValueContainer.h"
#include "UnlitMaterial.h"

#include "CoreUtils.h"

#define THROW_ERROR(hRes, error) \
if (FAILED(hRes)) {\
    throw error;\
}


namespace
{
	BasicObjectContainer<rendering::unlit_rp::UnlitRPTypeDef> m_unlitRP;
}

const rendering::unlit_rp::UnlitRPTypeDef& rendering::unlit_rp::UnlitRPTypeDef::GetTypeDef()
{
	if (!m_unlitRP.m_object)
	{
		m_unlitRP.m_object = new rendering::unlit_rp::UnlitRPTypeDef();
	}

	return *m_unlitRP.m_object;
}

rendering::unlit_rp::UnlitRPTypeDef::UnlitRPTypeDef() :
	ReferenceTypeDef(&render_pass::RenderPassTypeDef::GetTypeDef(), "DF88C6B2-118D-47D8-89B6-5AE3A6A61817")
{
	m_name = "Unlit";
	m_category = "Unlit RP";
}

rendering::unlit_rp::UnlitRPTypeDef::~UnlitRPTypeDef()
{
}

void rendering::unlit_rp::UnlitRPTypeDef::Construct(Value& container) const
{
	UnlitRP* unlitRP = new UnlitRP(*this);
	container.AssignObject(unlitRP);
}


rendering::unlit_rp::UnlitRP::UnlitRP(const ReferenceTypeDef& typeDef) :
	render_pass::RenderPass(typeDef),
	m_device(DXDeviceTypeDef::GetTypeDef(), this),
	m_swapChain(DXSwapChainTypeDef::GetTypeDef(), this),
	m_commandQueue(DXCommandQueueTypeDef::GetTypeDef(), this),
	m_unlitMaterial(UnlitMaterialTypeDef::GetTypeDef(), this)
{
}

rendering::unlit_rp::UnlitRP::~UnlitRP()
{
}

void rendering::unlit_rp::UnlitRP::Create()
{
	using Microsoft::WRL::ComPtr;

	DXDevice* device = m_device.GetValue<DXDevice*>();

	DXSwapChain* swapChain = rendering::core::utils::GetSwapChain();
	DXCommandQueue* commandQueue = rendering::core::utils::GetCommandQueue();

	m_swapChain.AssignObject(swapChain);
	m_commandQueue.AssignObject(commandQueue);

	THROW_ERROR(
		device->GetDevice().CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)),
		"Can't create Command Allocator!")

	THROW_ERROR(
		device->GetDevice().CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_beginCommandList)),
		"Can't create Command List!")

	THROW_ERROR(
		m_beginCommandList->Close(),
		"Can't close command List!")

	THROW_ERROR(
		device->GetDevice().CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_endCommandList)),
		"Can't create Command List!")

	THROW_ERROR(
		m_endCommandList->Close(),
		"Can't close command List!")
}

void rendering::unlit_rp::UnlitRP::Prepare()
{
	DXSwapChain* swapChain = m_swapChain.GetValue<DXSwapChain*>();
	int frameIndex = swapChain->GetCurrentSwapChainIndex();

	THROW_ERROR(
		m_commandAllocator->Reset(),
		"Can't reset Command Allocator!")
	
	THROW_ERROR(
		m_beginCommandList->Reset(m_commandAllocator.Get(), nullptr),
		"Can't reset Command List!")

	{
		CD3DX12_RESOURCE_BARRIER barrier[] =
		{
			CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(swapChain->GetCurrentRenderTarget(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET)
		};
		m_beginCommandList->ResourceBarrier(_countof(barrier), barrier);
	}

	THROW_ERROR(
		m_beginCommandList->Close(),
		"Can't close Command List!")

	THROW_ERROR(
		m_endCommandList->Reset(m_commandAllocator.Get(), nullptr),
		"Can't reset Command List!")

	{
		CD3DX12_RESOURCE_BARRIER barrier[] =
		{
			CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(swapChain->GetCurrentRenderTarget(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT)
		};
		m_beginCommandList->ResourceBarrier(_countof(barrier), barrier);
	}

	THROW_ERROR(
		m_endCommandList->Close(),
		"Can't close Command List!")
}

void rendering::unlit_rp::UnlitRP::Execute()
{
	DXCommandQueue* commandQueue = m_commandQueue.GetValue<DXCommandQueue*>();

	{
		ID3D12CommandList* commandLists[] = { m_beginCommandList.Get() };
		commandQueue->GetGraphicsCommandQueue()->ExecuteCommandLists(_countof(commandLists), commandLists);
	}

	{
		ID3D12CommandList* commandLists[] = { m_endCommandList.Get() };
		commandQueue->GetGraphicsCommandQueue()->ExecuteCommandLists(_countof(commandLists), commandLists);
	}
}

void rendering::unlit_rp::UnlitRP::Load(jobs::Job* done)
{
	auto getUnlitMaterial = [=]() {
		return m_unlitMaterial.GetValue<UnlitMaterial*>();
	};

	jobs::Job* loadUnlitMat = jobs::Job::CreateByLambda([=]() {
		UnlitMaterial* mat = getUnlitMaterial();
		mat->Load(done);
	});

	jobs::Job* init = jobs::Job::CreateByLambda([=]() {
		m_device.AssignObject(core::utils::GetDevice());
		m_swapChain.AssignObject(core::utils::GetSwapChain());
		m_commandQueue.AssignObject(core::utils::GetCommandQueue());

		m_unlitMaterial.AssignObject(ObjectValueContainer::GetObjectOfType(UnlitMaterialTypeDef::GetTypeDef()));

		Create();
		jobs::RunAsync(loadUnlitMat);
	});

	jobs::RunSync(init);
}


void rendering::unlit_rp::Boot()
{
	unlit_rp::UnlitRPTypeDef::GetTypeDef();
	unlit_rp::UnlitMaterialTypeDef::GetTypeDef();
}


#undef THROW_ERROR