#pragma once

#include "CompositeTypeDef.h"
#include "CompositeValue.h"

#include "Mesh.h"

#include "Job.h"

namespace scene
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

		MeshBuffers(const ReferenceTypeDef& typeDef);
		virtual ~MeshBuffers();

		void Load(geo::Mesh& mesh, jobs::Job* done);
	};
}