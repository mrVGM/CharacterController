#include "UnlitRP.h"

#include "PrimitiveTypes.h"

#include "Jobs.h"

#include "UnlitMaterial.h"

#include "ObjectValueContainer.h"
#include "UnlitMaterial.h"
#include "DisplayTextureMaterial.h"

#include "SceneObject.h"
#include "MeshBuffers.h"
#include "MeshActor.h"

#include "UnlitRenderTexture.h"

#include "ValueList.h"

#include "DXDevice.h"
#include "DXSwapChain.h"
#include "DXCommandQueue.h"
#include "DXDescriptorHeap.h"

#include <set>

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
	ReferenceTypeDef(&render_pass::RenderPassTypeDef::GetTypeDef(), "DF88C6B2-118D-47D8-89B6-5AE3A6A61817"),
	m_displayTextureMat("4399AADE-0D5B-4EB1-A733-EE7F9E09975A", TypeTypeDef::GetTypeDef(render_pass::DisplayTextureMaterialTypeDef::GetTypeDef())),
	m_quadMesh("CC292084-F703-427E-AD24-6B05386A0CDB", TypeTypeDef::GetTypeDef(geo::MeshTypeDef::GetTypeDef()))
{
	{
		m_displayTextureMat.m_name = "Display Unlit Texture Material";
		m_displayTextureMat.m_category = "Setup";
		m_displayTextureMat.m_getValue = [](CompositeValue* obj) -> Value& {
			UnlitRP* unlitRP = static_cast<UnlitRP*>(obj);
			return unlitRP->m_displayTextureMatDef;
		};
		m_properties[m_displayTextureMat.GetId()] = &m_displayTextureMat;
	}
	{
		m_quadMesh.m_name = "Quad Mesh";
		m_quadMesh.m_category = "Setup";
		m_quadMesh.m_getValue = [](CompositeValue* obj) -> Value& {
			UnlitRP* unlitRP = static_cast<UnlitRP*>(obj);
			return unlitRP->m_quadMeshDef;
		};
		m_properties[m_quadMesh.GetId()] = &m_quadMesh;
	}

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
	m_scene(scene::SceneObjectTypeDef::GetTypeDef(), this),

	m_displayTextureMatDef(UnlitRPTypeDef::GetTypeDef().m_displayTextureMat.GetType(), this),
	m_displayTextureMat(render_pass::DisplayTextureMaterialTypeDef::GetTypeDef(), this),
	m_quadMeshDef(UnlitRPTypeDef::GetTypeDef().m_quadMesh.GetType(), this),
	m_quadMesh(geo::MeshTypeDef::GetTypeDef(), this)
{
}

rendering::unlit_rp::UnlitRP::~UnlitRP()
{
	if (m_commandLists)
	{
		delete[] m_commandLists;
	}
	m_commandLists = nullptr;
}

void rendering::unlit_rp::UnlitRP::Create()
{
	using Microsoft::WRL::ComPtr;

	DXDevice* device = m_device.GetValue<DXDevice*>();

	Value swapChainVal(DXSwapChainTypeDef::GetTypeDef(), nullptr);
	Value commandQueueVal(DXCommandQueueTypeDef::GetTypeDef(), nullptr);
	ObjectValueContainer::GetObjectOfType(DXSwapChainTypeDef::GetTypeDef(), swapChainVal);
	ObjectValueContainer::GetObjectOfType(DXCommandQueueTypeDef::GetTypeDef(), commandQueueVal);

	DXSwapChain* swapChain = swapChainVal.GetValue<DXSwapChain*>();
	DXCommandQueue* commandQueue = commandQueueVal.GetValue<DXCommandQueue*>();

	m_swapChain.AssignObject(swapChain);
	m_commandQueue.AssignObject(commandQueue);

	THROW_ERROR(
		device->GetDevice().CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_auxCommandAllocator)),
		"Can't create Command Allocator!")

	for (int i = 0; i < 2; ++i)
	{
		THROW_ERROR(
			device->GetDevice().CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_auxCommandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_cache[i].m_beginCommandList)),
			"Can't create Command List!")

		THROW_ERROR(
			m_cache[i].m_beginCommandList->Close(),
			"Can't close command List!")

		THROW_ERROR(
			device->GetDevice().CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_auxCommandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_cache[i].m_afterRenderObjects)),
			"Can't create Command List!")

		THROW_ERROR(
			m_cache[i].m_afterRenderObjects->Close(),
			"Can't close command List!")

		THROW_ERROR(
			device->GetDevice().CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_auxCommandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_cache[i].m_endCommandList)),
			"Can't create Command List!")

		THROW_ERROR(
			m_cache[i].m_endCommandList->Close(),
			"Can't close command List!")

		THROW_ERROR(
			device->GetDevice().CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_auxCommandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_cache[i].m_displayRTCommandList)),
			"Can't create Command List!")

		THROW_ERROR(
			m_cache[i].m_displayRTCommandList->Close(),
			"Can't close command List!")
	}
}

void rendering::unlit_rp::UnlitRP::Prepare()
{
}

void rendering::unlit_rp::UnlitRP::Execute()
{
	DXCommandQueue* commandQueue = m_commandQueue.GetValue<DXCommandQueue*>();

	const CMDListCache& cache = GetCachedLists();
	{
		ID3D12CommandList* commandLists[] = { cache.m_beginCommandList.Get() };
		commandQueue->GetGraphicsCommandQueue()->ExecuteCommandLists(_countof(commandLists), commandLists);
	}

	DXDevice* device = m_device.GetValue<DXDevice*>();
	unlit_rp::UnlitMaterial* mat = m_unlitMaterial.GetValue<unlit_rp::UnlitMaterial*>();
	materials::Material* displayRTMat = m_displayTextureMat.GetValue<materials::Material*>();
	geo::Mesh* quadMesh = m_quadMesh.GetValue<geo::Mesh*>();

	scene::SceneObject* scene = m_scene.GetValue<scene::SceneObject*>();
	Value& actors = scene->GetActors();
	ValueList* actorList = actors.GetValue<ValueList*>();

	auto mapCommandLists = [=](std::function<void(ID3D12CommandList*)> f) {
		for (auto it = actorList->GetIterator(); it; ++it)
		{
			Value& curVal = *it;
			runtime::Actor* curActor = curVal.GetValue<runtime::Actor*>();
			if (!TypeDef::IsA(curActor->GetTypeDef(), runtime::MeshActorTypeDef::GetTypeDef()))
			{
				continue;
			}

			runtime::MeshActor* cur = static_cast<runtime::MeshActor*>(curActor);
			ValueList* matDefs = cur->m_materialDefs.GetValue<ValueList*>();

			std::set<const TypeDef*> matsProcessed;
			for (auto matIt = matDefs->GetIterator(); matIt; ++matIt)
			{
				Value& curMatType = *matIt;
				const TypeDef* matTypeDef = curMatType.GetType<const TypeDef*>();
				if (matsProcessed.contains(matTypeDef))
				{
					continue;
				}
				matsProcessed.insert(matTypeDef);

				std::list<ID3D12CommandList*> tmp;
				cur->GetCMDLists(curMatType.GetType<const TypeDef*>(), tmp);

				for (auto listIt = tmp.begin(); listIt != tmp.end(); ++listIt)
				{
					f(*listIt);
				}
			}
		}
	};

	int numLists = 0;
	mapCommandLists([&](ID3D12CommandList* cmdList) {
		++numLists;
	});

	if (numLists > m_commandListsSize)
	{
		ResizeCommandLists(numLists);
	}

	numLists = 0;
	mapCommandLists([&](ID3D12CommandList* cmdList) {
		m_commandLists[numLists++] = cmdList;
	});
	commandQueue->GetGraphicsCommandQueue()->ExecuteCommandLists(numLists, m_commandLists);

	{
		ID3D12CommandList* commandLists[] = { cache.m_afterRenderObjects.Get() };
		commandQueue->GetGraphicsCommandQueue()->ExecuteCommandLists(_countof(commandLists), commandLists);
	}

	{
		ID3D12CommandList* cmdLists[] = { cache.m_displayRTCommandList.Get() };
		m_commandQueue.GetValue<DXCommandQueue*>()->GetGraphicsCommandQueue()->ExecuteCommandLists(_countof(cmdLists), cmdLists);
	}

	{
		ID3D12CommandList* commandLists[] = { cache.m_endCommandList.Get() };
		commandQueue->GetGraphicsCommandQueue()->ExecuteCommandLists(_countof(commandLists), commandLists);
	}
}

void rendering::unlit_rp::UnlitRP::LoadData(jobs::Job* done)
{
	struct Context
	{
		int m_loading = 0;
	};
	Context* ctx = new Context();

	auto itemLoaded = [=]() {
		--ctx->m_loading;
		if (ctx->m_loading > 0)
		{
			return;
		}
		delete ctx;

		jobs::RunSync(done);
	};

	jobs::Job* loadDisplayTextureMat = jobs::Job::CreateByLambda([=]() {
		materials::Material* mat = m_displayTextureMat.GetValue<materials::Material*>();
		mat->Load(jobs::Job::CreateByLambda(itemLoaded));
	});

	jobs::Job* loadQuadMesh = jobs::Job::CreateByLambda([=]() {
		geo::Mesh* mesh = m_quadMesh.GetValue<geo::Mesh*>();

		jobs::Job* loadMeshBuffers = jobs::Job::CreateByLambda([=]() {
			runtime::MeshBuffersTypeDef::GetTypeDef().Construct(mesh->m_buffers);
			runtime::MeshBuffers* meshBuffers = mesh->m_buffers.GetValue<runtime::MeshBuffers*>();

			jobs::RunAsync(jobs::Job::CreateByLambda([=]() {
				meshBuffers->Load(*mesh, jobs::Job::CreateByLambda(itemLoaded));
			}));
		});

		mesh->Load(loadMeshBuffers);
	});

	jobs::Job* init = jobs::Job::CreateByLambda([=]() {
		ObjectValueContainer::GetObjectOfType(DXDeviceTypeDef::GetTypeDef(), m_device);
		ObjectValueContainer::GetObjectOfType(DXSwapChainTypeDef::GetTypeDef(), m_swapChain);
		ObjectValueContainer::GetObjectOfType(DXCommandQueueTypeDef::GetTypeDef(), m_commandQueue);

		ObjectValueContainer::GetObjectOfType(scene::SceneObjectTypeDef::GetTypeDef(), m_scene);

		ObjectValueContainer::GetObjectOfType(UnlitMaterialTypeDef::GetTypeDef(), m_unlitMaterial);
		ObjectValueContainer::GetObjectOfType(*m_displayTextureMatDef.GetType<const TypeDef*>(), m_displayTextureMat);
		ObjectValueContainer::GetObjectOfType(*m_quadMeshDef.GetType<const TypeDef*>(), m_quadMesh);

		Create();
		++ctx->m_loading;
		jobs::RunAsync(loadDisplayTextureMat);
		++ctx->m_loading;
		jobs::RunAsync(loadQuadMesh);
	});

	jobs::RunSync(init);
}

const rendering::unlit_rp::UnlitRP::CMDListCache& rendering::unlit_rp::UnlitRP::GetCachedLists()
{
	DXSwapChain* swapChain = m_swapChain.GetValue<DXSwapChain*>();
	UINT frameIndex = swapChain->GetCurrentSwapChainIndex();

	DXTexture* rt = nullptr;
	DXDescriptorHeap* rtHeap = nullptr;
	{
		UnlitMaterial* mat = m_unlitMaterial.GetValue<UnlitMaterial*>();
		const Value& rtHeapVal = mat->GetRTHeap();
		rtHeap = rtHeapVal.GetValue<DXDescriptorHeap*>();
		const Value& textures = rtHeap->GetTextures();
		ValueList* l = textures.GetValue<ValueList*>();
		const Value& fst = *(l->GetIterator());
		rt = fst.GetValue<DXTexture*>();
	}

	CMDListCache& cache = m_cache[frameIndex];
	if (cache.m_cached)
	{
		return cache;
	}

	THROW_ERROR(
		cache.m_beginCommandList->Reset(m_auxCommandAllocator.Get(), nullptr),
		"Can't reset Command List!")

	{
		CD3DX12_RESOURCE_BARRIER barrier[] =
		{
			CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(rt->GetTexture(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET),
			CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(swapChain->GetCurrentRenderTarget(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET)
		};
		cache.m_beginCommandList->ResourceBarrier(_countof(barrier), barrier);

		const float clearColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		cache.m_beginCommandList->ClearRenderTargetView(rtHeap->GetDescriptorHandle(0), clearColor, 0, nullptr);
	}

	THROW_ERROR(
		cache.m_beginCommandList->Close(),
		"Can't close Command List!")

	THROW_ERROR(
		cache.m_afterRenderObjects->Reset(m_auxCommandAllocator.Get(), nullptr),
		"Can't reset Command List!")

	{
		CD3DX12_RESOURCE_BARRIER barrier[] =
		{
			CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(rt->GetTexture(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE)
		};
		cache.m_afterRenderObjects->ResourceBarrier(_countof(barrier), barrier);
	}

	THROW_ERROR(
		cache.m_afterRenderObjects->Close(),
		"Can't close Command List!")

	THROW_ERROR(
		cache.m_endCommandList->Reset(m_auxCommandAllocator.Get(), nullptr),
		"Can't reset Command List!")

	{
		CD3DX12_RESOURCE_BARRIER barrier[] =
		{
			CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(rt->GetTexture(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_PRESENT),
			CD3DX12_RESOURCE_BARRIER::CD3DX12_RESOURCE_BARRIER::Transition(swapChain->GetCurrentRenderTarget(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT)
		};
		cache.m_endCommandList->ResourceBarrier(_countof(barrier), barrier);
	}

	THROW_ERROR(
		cache.m_endCommandList->Close(),
		"Can't close Command List!")


	{
		materials::Material* displayRTMat = m_displayTextureMat.GetValue<materials::Material*>();
		geo::Mesh* quadMesh = m_quadMesh.GetValue<geo::Mesh*>();
		runtime::MeshBuffers* meshBuffers = quadMesh->m_buffers.GetValue<runtime::MeshBuffers*>();
		DXBuffer* vertexBuffer = meshBuffers->m_vertexBuffer.GetValue<DXBuffer*>();
		DXBuffer* indexBuffer = meshBuffers->m_indexBuffer.GetValue<DXBuffer*>();

		const geo::Mesh::MaterialRange& range = quadMesh->m_materials.front();

		displayRTMat->GenerateCommandList(
			*vertexBuffer,
			*indexBuffer,
			*vertexBuffer,
			range.m_start,
			range.m_count,
			m_auxCommandAllocator.Get(),
			cache.m_displayRTCommandList.Get()
		);

	}

	cache.m_cached = true;
	return cache;
}

void rendering::unlit_rp::UnlitRP::ResizeCommandLists(int size)
{
	if (m_commandLists)
	{
		delete[] m_commandLists;
	}

	m_commandListsSize = size;
	m_commandLists = new ID3D12CommandList* [m_commandListsSize];
}


void rendering::unlit_rp::Boot()
{
	unlit_rp::UnlitRPTypeDef::GetTypeDef();
	unlit_rp::UnlitMaterialTypeDef::GetTypeDef();
	unlit_rp::UnlitRenderTextureTypeDef::GetTypeDef();
}


#undef THROW_ERROR