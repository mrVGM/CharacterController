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

namespace scene
{
	class ActorTypeDef : public ReferenceTypeDef
	{
		TYPE_DEF_BODY(ActorTypeDef)

	public:
		TypeProperty m_mesh;
		TypeProperty m_materials;

		ActorTypeDef();
		virtual ~ActorTypeDef();

		void Construct(Value& container) const override;
	};

	class Actor : public ObjectValue, jobs::LoadingClass
	{
	private:
		jobs::MultiLoader m_loader;

		Value m_device;
		Value m_materials;

		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
		std::map<
			const rendering::materials::MaterialTypeDef*, 
			std::list<Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> > > m_cmdListCache;

		void CacheCMDLists(jobs::Job* done);
	protected:
		virtual void LoadData(jobs::Job* done) override;

	public:
		Value m_mesh;
		Value m_meshDef;
		Value m_materialDefs;

		Actor(const ReferenceTypeDef& typeDef);
		virtual ~Actor();

		void SetMesh(geo::Mesh* mesh);

		void Load(jobs::Job* done);

		void GetCMDLists(const rendering::materials::MaterialTypeDef* material, std::list<ID3D12CommandList*>& outLists);
	};
}