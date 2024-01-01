#pragma once

#include "CompositeTypeDef.h"
#include "CompositeValue.h"

#include "Mesh.h"
#include "Material.h"

#include "MultiLoader.h"
#include "Job.h"

#include "d3dx12.h"

#include<wrl.h>
#include <map>
#include <list>

namespace runtime
{
	class ActorTypeDef : public ReferenceTypeDef
	{
		TYPE_DEF_BODY(ActorTypeDef)

	public:
		TypeProperty m_mesh;
		TypeProperty m_animator;
		TypeProperty m_skeleton;
		TypeProperty m_materials;

		ActorTypeDef();
		virtual ~ActorTypeDef();

		void Construct(Value& container) const override;
	};

	class Actor : public ObjectValue, public jobs::LoadingClass
	{
	private:
		Value m_device;
		Value m_materials;

		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
		std::map<
			const TypeDef*, 
			std::list<Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> > > m_cmdListCache;

		void CacheCMDLists(jobs::Job* done);
	protected:
		virtual void LoadData(jobs::Job* done) override;

	public:
		Value m_meshDef;
		Value m_skeletonDef;
		Value m_materialDefs;
		Value m_animatorDef;

		Value m_mesh;
		Value m_skeleton;
		Value m_transformBuffer;
		Value m_poseBuffer;
		Value m_animator;

		Actor(const ReferenceTypeDef& typeDef);
		virtual ~Actor();

		void SetMesh(geo::Mesh* mesh);

		void GetCMDLists(const TypeDef* material, std::list<ID3D12CommandList*>& outLists);
	};
}