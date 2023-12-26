#pragma once

#include "CompositeTypeDef.h"
#include "CompositeValue.h"

#include "Mesh.h"

#include "Job.h"

namespace scene
{
	class ActorTypeDef : public ReferenceTypeDef
	{
		TYPE_DEF_BODY(ActorTypeDef)

	public:
		TypeProperty m_mesh;

		ActorTypeDef();
		virtual ~ActorTypeDef();

		void Construct(Value& container) const override;
	};

	class Actor : public ObjectValue
	{
	public:
		Value m_mesh;

		Actor(const ReferenceTypeDef& typeDef);
		virtual ~Actor();

		void SetMesh(geo::Mesh* mesh);

		void Load(jobs::Job* done);
	};
}