#pragma once

#include "CompositeTypeDef.h"
#include "CompositeValue.h"

#include "Geometry.h"

#include "MultiLoader.h"
#include "Job.h"

#include "MemoryFile.h"

namespace xml_reader
{
	struct XMLTree;
	struct Node;
}

namespace geo
{
	class MeshTypeDef : public ReferenceTypeDef
	{
		TYPE_DEF_BODY(MeshTypeDef)

	public:
		TypeProperty m_colladaFile;
		TypeProperty m_hash;

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

		struct SkinData
		{
			std::vector<std::string> m_boneNames;
			math::Matrix m_bindShapeMatrix;
			std::vector<math::Matrix> m_invBindMatrices;
		};

		Value m_colladaFile;
		Value m_hash;
		Value m_buffers;

		bool m_zUp = false;

		int m_numVertices = -1;
		MeshVertex* m_vertices = nullptr;

		int m_numIndices = -1;
		int* m_indices = nullptr;
		
		std::list<MaterialRange> m_materials;

		SkinData* m_skinData = nullptr;

		Mesh(const ReferenceTypeDef& type);
		virtual ~Mesh();

		void Load(jobs::Job* done);
	};
}