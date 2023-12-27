#include "UnlitRP.h"

#include "Jobs.h"

#include "UnlitMaterial.h"

#include "ObjectValueContainer.h"
#include "UnlitMaterial.h"

#include "SceneObject.h"
#include "MeshBuffers.h"
#include "Actor.h"

#include "UnlitRenderTextureTexture.h"

#include "ValueList.h"

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
	m_unlitMaterial(UnlitMaterialTypeDef::GetTypeDef(), this),
	m_scene(scene::SceneObjectTypeDef::GetTypeDef(), this)
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
	m_commandLists.clear();

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
		m_endCommandList->ResourceBarrier(_countof(barrier), barrier);
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

	DXDevice* device = m_device.GetValue<DXDevice*>();
	unlit_rp::UnlitMaterial* mat = m_unlitMaterial.GetValue<unlit_rp::UnlitMaterial*>();

	scene::SceneObject* scene = m_scene.GetValue<scene::SceneObject*>();
	Value& actors = scene->GetActors();
	ValueList* actorList = actors.GetValue<ValueList*>();
	for (auto it = actorList->GetIterator(); it; ++it)
	{
		scene::Actor* cur = (*it).GetValue<scene::Actor*>();
		geo::Mesh* mesh = cur->m_mesh.GetValue<geo::Mesh*>();

		scene::MeshBuffers* meshBuffers = mesh->m_buffers.GetValue<scene::MeshBuffers*>();

		DXBuffer* vertexBuffer = meshBuffers->m_vertexBuffer.GetValue<DXBuffer*>();
		DXBuffer* indexBuffer = meshBuffers->m_indexBuffer.GetValue<DXBuffer*>();

		for (auto it = mesh->m_materials.begin(); it != mesh->m_materials.end(); ++it)
		{
			geo::Mesh::MaterialRange& matRange = *it;

			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList = m_commandLists.emplace_back();
			
			THROW_ERROR(
				device->GetDevice().CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&cmdList)),
				"Can't create Command List!")

			THROW_ERROR(
				cmdList->Close(),
				"Can't close Command List!")

			mat->GenerateCommandList(
				*vertexBuffer,
				*indexBuffer,
				*vertexBuffer,
				matRange.m_start,
				matRange.m_count,
				m_commandAllocator.Get(),
				cmdList.Get());


			ID3D12CommandList* cmdLists[] = { cmdList.Get() };
			m_commandQueue.GetValue<DXCommandQueue*>()->GetGraphicsCommandQueue()->ExecuteCommandLists(_countof(cmdLists), cmdLists);
		}
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

		m_scene.AssignObject(ObjectValueContainer::GetObjectOfType(scene::SceneObjectTypeDef::GetTypeDef()));
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
	unlit_rp::UnlitRenderTextureTypeDef::GetTypeDef();
}


#undef THROW_ERROR