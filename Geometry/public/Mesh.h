#pragma once

#include "CompositeTypeDef.h"
#include "CompositeValue.h"

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
		Value m_colladaFile;

		Mesh(const CompositeTypeDef& type, const CompositeValue* outer);

		void Load(jobs::Job* done);
	};
}