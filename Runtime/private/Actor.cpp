#include "Actor.h"

#include "PrimitiveTypes.h"
#include "ListDef.h"
#include "ValueList.h"
#include "Material.h"

#include "Mesh.h"
#include "Skeleton.h"
#include "MeshBuffers.h"
#include "Animator.h"

#include "Jobs.h"

#include "ObjectValueContainer.h"

#include "DXMutableBuffer.h"

#include "AssetTypeDef.h"

#include "CoreUtils.h"

#define THROW_ERROR(hRes, error) \
if (FAILED(hRes)) {\
    throw error;\
}

namespace
{
	BasicObjectContainer<runtime::ActorTypeDef> m_actorTypeDef;
}

const runtime::ActorTypeDef& runtime::ActorTypeDef::GetTypeDef()
{
	if (!m_actorTypeDef.m_object)
	{
		m_actorTypeDef.m_object = new ActorTypeDef();
	}

	return *m_actorTypeDef.m_object;
}

runtime::ActorTypeDef::ActorTypeDef() :
	ReferenceTypeDef(&ReferenceTypeDef::GetTypeDef(), "9CFFA9B0-3542-4509-81CD-387C496ADAFF"),
	m_mesh("6B501787-EFF6-4EDF-85A4-571428A47071", TypeTypeDef::GetTypeDef(geo::MeshTypeDef::GetTypeDef())),
	m_animator("C2D55EEE-8279-4CA5-BDA9-699FA696EF19", TypeTypeDef::GetTypeDef(animation::AnimatorTypeDef::GetTypeDef())),
	m_skeleton("27391D76-9D4F-4490-B3F3-0DD453CE22A5", TypeTypeDef::GetTypeDef(geo::SkeletonTypeDef::GetTypeDef())),
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
		m_animator.m_name = "Animator";
		m_animator.m_category = "Setup";
		m_animator.m_getValue = [](CompositeValue* obj) -> Value& {
			Actor* actor = static_cast<Actor*>(obj);
			return actor->m_animatorDef;
		};
		m_properties[m_animator.GetId()] = &m_animator;
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

	{
		m_skeleton.m_name = "Skeleton";
		m_skeleton.m_category = "Setup";
		m_skeleton.m_getValue = [](CompositeValue* obj) -> Value& {
			Actor* actor = static_cast<Actor*>(obj);
			return actor->m_skeletonDef;
		};
		m_properties[m_skeleton.GetId()] = &m_skeleton;
	}

	m_name = "Actor";
	m_category = "Scene";
}

runtime::ActorTypeDef::~ActorTypeDef()
{
}

void runtime::ActorTypeDef::Construct(Value& container) const
{
	Actor* actor = new Actor(*this);
	container.AssignObject(actor);
}

runtime::Actor::Actor(const ReferenceTypeDef& typeDef) :
	ObjectValue(typeDef),
	m_device(rendering::DXDeviceTypeDef::GetTypeDef(), this),
	m_meshDef(ActorTypeDef::GetTypeDef().m_mesh.GetType(), this),
	m_mesh(geo::MeshTypeDef::GetTypeDef(), this),
	m_skeletonDef(ActorTypeDef::GetTypeDef().m_skeleton.GetType(), this),
	m_skeleton(geo::SkeletonTypeDef::GetTypeDef(), this),
	m_materialDefs(ActorTypeDef::GetTypeDef().m_materials.GetType(), this),
	m_materials(ListDef::GetTypeDef(rendering::materials::MaterialTypeDef::GetTypeDef()), this),
	m_transformBuffer(rendering::DXMutableBufferTypeDef::GetTypeDef(), this),
	m_poseBuffer(rendering::DXMutableBufferTypeDef::GetTypeDef(), this),
	m_animatorDef(ActorTypeDef::GetTypeDef().m_animator.GetType(), this),
	m_animator(animation::AnimatorTypeDef::GetTypeDef(), this)
{
}

runtime::Actor::~Actor()
{
}

void runtime::Actor::SetMesh(geo::Mesh* mesh)
{
	m_mesh.AssignObject(mesh);
}

void runtime::Actor::LoadData(jobs::Job* done)
{
	struct Context
	{
		int m_loading = 0;
	};

	Context* ctx = new Context();

	jobs::Job* initAnimator = jobs::Job::CreateByLambda([=]() {
		animation::Animator* animator = m_animator.GetValue<animation::Animator*>();
		if (animator)
		{
			animator->SetActor(*this);
		}
		jobs::RunSync(done);
	});

	auto itemLoaded = [=]() {
		--ctx->m_loading;
		if (ctx->m_loading > 0)
		{
			return;
		}
		delete ctx;

		CacheCMDLists(initAnimator);
	};

	jobs::Job* initMeshBuffers = jobs::Job::CreateByLambda([=]() {
		geo::Mesh* mesh = m_mesh.GetValue<geo::Mesh*>();
		MeshBuffersTypeDef::GetTypeDef().Construct(mesh->m_buffers);
		MeshBuffers* meshBuffers = mesh->m_buffers.GetValue<MeshBuffers*>();

		jobs::RunAsync(jobs::Job::CreateByLambda([=]() {
			meshBuffers->Load(*mesh, jobs::Job::CreateByLambda([=]() {
				if (!mesh->m_skinData.m_hasAnyData)
				{
					jobs::RunSync(jobs::Job::CreateByLambda(itemLoaded));
					return;
				}

				rendering::DXMutableBufferTypeDef::GetTypeDef().Construct(m_poseBuffer);
				rendering::DXMutableBuffer* poseBuffer = m_poseBuffer.GetValue<rendering::DXMutableBuffer*>();

				int poseBuffStride = sizeof(math::Matrix);
				int poseBuffSize = mesh->m_skinData.m_boneNames.size() * poseBuffStride;
				poseBuffer->SetSizeAndStride(poseBuffSize, poseBuffStride);
				jobs::RunAsync(jobs::Job::CreateByLambda([=]() {
					poseBuffer->Load(jobs::Job::CreateByLambda(itemLoaded));
				}));
			}));
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

		rendering::DXMutableBufferTypeDef::GetTypeDef().Construct(m_transformBuffer);
		rendering::DXMutableBuffer* transformBuffer = m_transformBuffer.GetValue<rendering::DXMutableBuffer*>();
		transformBuffer->SetSizeAndStride(256, 256);
		++ctx->m_loading;
		jobs::RunAsync(jobs::Job::CreateByLambda([=]() {
			transformBuffer->Load(jobs::Job::CreateByLambda(itemLoaded));
		}));

		const TypeDef* skeletonDef = m_skeletonDef.GetType<const TypeDef*>();
		if (skeletonDef)
		{
			m_skeleton.AssignObject(ObjectValueContainer::GetObjectOfType(*skeletonDef));
		}
		geo::Skeleton* skeleton = m_skeleton.GetValue<geo::Skeleton*>();

		if (skeleton)
		{
			++ctx->m_loading;
			jobs::RunAsync(jobs::Job::CreateByLambda([=]() {
				skeleton->Load(jobs::Job::CreateByLambda(itemLoaded));
			}));


			const AssetTypeDef* animatorAssetDef = static_cast<const AssetTypeDef*>(m_animatorDef.GetType<const TypeDef*>());
			const TypeDef* animatorDef = animatorAssetDef;
			if (!animatorDef)
			{
				animatorDef = &animation::AnimatorTypeDef::GetTypeDef();
			}
			static_cast<const CompositeTypeDef*>(animatorDef)->Construct(m_animator);
			if (animatorAssetDef)
			{
				AssetTypeDef* asset = const_cast<AssetTypeDef*>(animatorAssetDef);
				auto& map = asset->m_data.GetAsObj();
				asset->DeserializeFromJSON(m_animator, map["defaults"]);
			}

			animation::Animator* animator = m_animator.GetValue<animation::Animator*>();

			++ctx->m_loading;
			animator->Load(jobs::Job::CreateByLambda(itemLoaded));
		}

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

void runtime::Actor::CacheCMDLists(jobs::Job* done)
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

			std::list<Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> >& cmdLists = m_cmdListCache[&material->GetTypeDef()];
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

			if (!mesh->m_skinData.m_hasAnyData)
			{
			
				material->GenerateCommandList(
					*vertexBuffer,
					*indexBuffer,
					*vertexBuffer,
					range.m_start,
					range.m_count,
					m_commandAllocator.Get(),
					cmdList.Get());
			}
			else
			{
				rendering::DXBuffer* weightsIdBuffer = meshBuffers->m_vertexWeightsMapBuffer.GetValue<rendering::DXBuffer*>();
				rendering::DXBuffer* weightsBuffer = meshBuffers->m_vertexWeightsBuffer.GetValue<rendering::DXBuffer*>();
				rendering::DXBuffer* bindShapeBuffer = meshBuffers->m_bindShapeBuffer.GetValue<rendering::DXBuffer*>();

				rendering::DXMutableBuffer* poseMutBuffer = m_poseBuffer.GetValue<rendering::DXMutableBuffer*>();
				rendering::DXBuffer* poseBuffer = poseMutBuffer->m_buffer.GetValue<rendering::DXBuffer*>();

				material->GenerateCommandList(
					*vertexBuffer,
					*indexBuffer,
					*vertexBuffer,

					*weightsIdBuffer,
					*weightsBuffer,
					*bindShapeBuffer,
					*poseBuffer,

					range.m_start,
					range.m_count,
					m_commandAllocator.Get(),
					cmdList.Get());
			}
		}

		jobs::RunSync(done);
	});

	jobs::RunAsync(recordCommandLists);
}

void runtime::Actor::GetCMDLists(const TypeDef* material, std::list<ID3D12CommandList*>& outLists)
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