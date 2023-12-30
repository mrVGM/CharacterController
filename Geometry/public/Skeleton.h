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
	class SkeletonTypeDef : public ReferenceTypeDef
	{
		TYPE_DEF_BODY(SkeletonTypeDef)

	public:
		TypeProperty m_colladaFile;
		TypeProperty m_hash;

		SkeletonTypeDef();
		virtual ~SkeletonTypeDef();

		void Construct(Value& container) const override;
	};


	class Skeleton : public ObjectValue, public jobs::LoadingClass
	{
	private:
		jobs::MultiLoader m_loader;

	protected:
		virtual void LoadData(jobs::Job* done) override;

	public:
		Value m_colladaFile;
		Value m_hash;
		Value m_buffers;

#pragma region Collada read helpers
		
		bool m_zUp = false;

#pragma endregion

#pragma region Binary Serialized

		std::vector<std::string> m_boneNames;
		std::vector<int> m_boneParents;
		std::vector<math::Matrix> m_bindPose;

#pragma endregion

		Skeleton(const ReferenceTypeDef& type);
		virtual ~Skeleton();

		void Load(jobs::Job* done);
	};
}