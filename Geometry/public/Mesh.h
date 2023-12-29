#pragma once

#include "CompositeTypeDef.h"
#include "CompositeValue.h"

#include "Geometry.h"

#include "MultiLoader.h"
#include "Job.h"

#include "MemoryFile.h"

namespace geo
{
	class MeshTypeDef : public ReferenceTypeDef
	{
		TYPE_DEF_BODY(MeshTypeDef)

	public:
		TypeProperty m_colladaFile;

		MeshTypeDef();
		virtual ~MeshTypeDef();

		void Construct(Value& container) const override;
	};


	class Mesh : public ObjectValue, public jobs::LoadingClass
	{
	private:
		jobs::MultiLoader m_loader;

		void SerializeToMF(files::MemoryFile& mf);
		void DeserializeFromMF(files::MemoryFile& mf);

	protected:
		virtual void LoadData(jobs::Job* done) override;

	public:
		struct MaterialRange
		{
			int m_start = -1;
			int m_count = -1;
		};

		Value m_colladaFile;
		Value m_buffers;

		bool m_zUp = false;

		int m_numVertices = -1;
		MeshVertex* m_vertices = nullptr;

		int m_numIndices = -1;
		int* m_indices = nullptr;
		
		std::list<MaterialRange> m_materials;

		Mesh(const ReferenceTypeDef& type);
		virtual ~Mesh();

		void Load(jobs::Job* done);
	};
}