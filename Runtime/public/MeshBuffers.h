#pragma once

#include "CompositeTypeDef.h"
#include "CompositeValue.h"

#include "Mesh.h"

#include "Job.h"

namespace runtime
{
	class MeshBuffersTypeDef : public ReferenceTypeDef
	{
		TYPE_DEF_BODY(MeshBuffersTypeDef)

	public:
		MeshBuffersTypeDef();
		virtual ~MeshBuffersTypeDef();

		void Construct(Value& container) const override;
	};

	class MeshBuffers : public ObjectValue
	{
	public:
		Value m_vertexBuffer;
		Value m_indexBuffer;

		Value m_vertexWeightsMapBuffer;
		Value m_vertexWeightsBuffer;

		MeshBuffers(const ReferenceTypeDef& typeDef);
		virtual ~MeshBuffers();

		void Load(geo::Mesh& mesh, jobs::Job* done);
	};
}