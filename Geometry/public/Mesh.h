#pragma once

#include "CompositeTypeDef.h"
#include "CompositeValue.h"

#include "Geometry.h"

#include "Job.h"

namespace geo
{
	class MeshTypeDef : public ReferenceTypeDef
	{
		TYPE_DEF_BODY(MeshTypeDef)

	public:
		TypeProperty m_colladaFile;

		MeshTypeDef();

		void Construct(Value& container) const override;
	};


	class Mesh : public ObjectValue
	{
	public:
		struct MaterialRange
		{
			int m_start = -1;
			int m_count = -1;
		};

		Value m_colladaFile;

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