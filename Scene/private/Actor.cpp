#include "Actor.h"

#include "PrimitiveTypes.h"
#include "ListDef.h"
#include "ValueList.h"
#include "Material.h"

#include "Mesh.h"
#include "MeshBuffers.h"

#include "Jobs.h"

#include "ObjectValueContainer.h"

#include "CoreUtils.h"

#define THROW_ERROR(hRes, error) \
if (FAILED(hRes)) {\
    throw error;\
}

namespace
{
	BasicObjectContainer<scene::ActorTypeDef> m_actorTypeDef;
}

const scene::ActorTypeDef& scene::ActorTypeDef::GetTypeDef()
{
	if (!m_actorTypeDef.m_object)
	{
		m_actorTypeDef.m_object = new ActorTypeDef();
	}

	return *m_actorTypeDef.m_object;
}

scene::ActorTypeDef::ActorTypeDef() :
	ReferenceTypeDef(&ReferenceTypeDef::GetTypeDef(), "9CFFA9B0-3542-4509-81CD-387C496ADAFF"),
	m_mesh("6B501787-EFF6-4EDF-85A4-571428A47071", TypeTypeDef::GetTypeDef(geo::MeshTypeDef::GetTypeDef())),
	m_materials("2A8E8B5A-2CCD-4957-ABDB-6685AA5A8DBF", ListDef::GetTypeDef(TypeTypeDef::GetTypeDef(rendering::materials::MaterialTypeDef::GetTypeDef())))
{
	{
		m_mesh.m_name = "Mesh";
		m_mesh.m_category = "Setup";
		m_mesh.m_getValue = [](CompositeValue* obj) -> Value& {
			Actor* actor = static_cast<Actor*>(obj);
			return actor->m_meshDef;
		};
		m_properties[m_mesh.GetId()] = &m_mesh;
	}

	{
		m_materials.m_name = "Materials";
		m_materials.m_category = "Setup";
		m_materials.m_getValue = [](CompositeValue* obj) -> Value& {
			Actor* actor = static_cast<Actor*>(obj);
			return actor->m_materialDefs;
		};
		m_properties[m_materials.GetId()] = &m_materials;
	}

	m_name = "Actor";
	m_category = "Scene";
}

scene::ActorTypeDef::~ActorTypeDef()
{
}

void scene::ActorTypeDef::Construct(Value& container) const
{
	Actor* actor = new Actor(*this);
	container.AssignObject(actor);
}

scene::Actor::Actor(const ReferenceTypeDef& typeDef) :
	ObjectValue(typeDef),
	m_loader(*this),
	m_device(rendering::DXDeviceTypeDef::GetTypeDef(), this),
	m_meshDef(ActorTypeDef::GetTypeDef().m_mesh.GetType(), this),
	m_mesh(geo::MeshTypeDef::GetTypeDef(), this),
	m_materialDefs(ActorTypeDef::GetTypeDef().m_materials.GetType(), this),
	m_materials(ListDef::GetTypeDef(rendering::materials::MaterialTypeDef::GetTypeDef()), this)
{
}

scene::Actor::~Actor()
{
}

void scene::Actor::SetMesh(geo::Mesh* mesh)
{
	m_mesh.AssignObject(mesh);
}

void scene::Actor::Load(jobs::Job* done)
{
	m_loader.Load(done);
}

void scene::Actor::LoadData(jobs::Job* done)
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

		CacheCMDLists(done);
	};

	jobs::Job* initMeshBuffers = jobs::Job::CreateByLambda([=]() {
		geo::Mesh* mesh = m_mesh.GetValue<geo::Mesh*>();
		MeshBuffersTypeDef::GetTypeDef().Construct(mesh->m_buffers);
		MeshBuffers* meshBuffers = mesh->m_buffers.GetValue<MeshBuffers*>();

		jobs::RunAsync(jobs::Job::CreateByLambda([=]() {
			meshBuffers->Load(*mesh, jobs::Job::CreateByLambda(itemLoaded));
		}));
	});

	auto loadMat = [=](rendering::materials::Material* material) {
		return jobs::Job::CreateByLambda([=]() {
			material->Load(jobs::Job::CreateByLambda(itemLoaded));
		});
	};

	jobs::Job* init = jobs::Job::CreateByLambda([=]() {
		m_device.AssignObject(rendering::core::utils::GetDevice());

		m_mesh.AssignObject(ObjectValueContainer::GetObjectOfType(*m_meshDef.GetType<const TypeDef*>()));
		geo::Mesh* mesh = m_mesh.GetValue<geo::Mesh*>();

		++ctx->m_loading;
		jobs::RunAsync(jobs::Job::CreateByLambda([=]() {
			mesh->Load(initMeshBuffers);
		}));

		ValueList* matDefs = m_materialDefs.GetValue<ValueList*>();
		ValueList* mats = m_materials.GetValue<ValueList*>();

		for (auto it = matDefs->GetIterator(); it; ++it)
		{
			const Value& cur = *it;

			Value& mat = mats->EmplaceBack();
			mat.AssignObject(ObjectValueContainer::GetObjectOfType(*cur.GetType<const TypeDef*>()));

			rendering::materials::Material* material = mat.GetValue<rendering::materials::Material*>();

			++ctx->m_loading;
			jobs::RunAsync(loadMat(material));
		}
	});

	jobs::RunSync(init);
}

void scene::Actor::CacheCMDLists(jobs::Job* done)
{
	jobs::Job* recordCommandLists = jobs::Job::CreateByLambda([=]() {
		rendering::DXDevice* device = m_device.GetValue<rendering::DXDevice*>();

		THROW_ERROR(
			device->GetDevice().CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)),
			"Can't create Command Allocator!")

		geo::Mesh* mesh = m_mesh.GetValue<geo::Mesh*>();
		ValueList* materials = m_materials.GetValue<ValueList*>();

		auto matIt = materials->GetIterator();

		for (auto it = mesh->m_materials.begin(); it != mesh->m_materials.end(); ++it)
		{
			if (!matIt)
			{
				break;
			}
			Value& curMat = *matIt;
			++matIt;

			const geo::Mesh::MaterialRange& range = *it;
			rendering::materials::Material* material = curMat.GetValue<rendering::materials::Material*>();
			const rendering::materials::MaterialTypeDef* matTypeDef = 
				static_cast<const rendering::materials::MaterialTypeDef*>(&material->GetTypeDef());

			std::list<Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> >& cmdLists = m_cmdListCache[matTypeDef];
			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList = cmdLists.emplace_back();

			THROW_ERROR(
				device->GetDevice().CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&cmdList)),
				"Can't create Command List!")

			THROW_ERROR(
				cmdList->Close(),
				"Can't close Command List!")

			MeshBuffers* meshBuffers = mesh->m_buffers.GetValue<MeshBuffers*>();
			rendering::DXBuffer* vertexBuffer = meshBuffers->m_vertexBuffer.GetValue<rendering::DXBuffer*>();
			rendering::DXBuffer* indexBuffer = meshBuffers->m_indexBuffer.GetValue<rendering::DXBuffer*>();
			
			material->GenerateCommandList(
				*vertexBuffer,
				*indexBuffer,
				*vertexBuffer,
				range.m_start,
				range.m_start,
				m_commandAllocator.Get(),
				cmdList.Get());
		}

		jobs::RunSync(done);
	});

	jobs::RunAsync(recordCommandLists);
}

void scene::Actor::GetCMDLists(const rendering::materials::MaterialTypeDef* material, std::list<ID3D12CommandList*>& outLists)
{
	auto it = m_cmdListCache.find(material);
	if (it == m_cmdListCache.end())
	{
		return;
	}

	for (auto listIt = it->second.begin(); listIt != it->second.end(); ++listIt)
	{
		outLists.push_back((*listIt).Get());
	}
}

#undef THROW_ERROR