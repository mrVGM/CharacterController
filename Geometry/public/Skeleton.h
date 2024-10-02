#pragma once

#include "CompositeTypeDef.h"
#include "CompositeValue.h"

#include "Geometry.h"

#include "MultiLoader.h"
#include "Jobs.h"

#include "MemoryFile.h"

#include <map>

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
		void SerializeToMF(files::MemoryFile& mf);
		void DeserializeFromMF(files::MemoryFile& mf);

		std::map<std::string, int> m_boneIndices;

	protected:
		virtual void LoadData(jobs::Job done) override;

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

		int GetBoneIndex(const std::string& boneName) const;
	};
}