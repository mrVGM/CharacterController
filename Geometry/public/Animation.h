#pragma once

#include "CompositeTypeDef.h"
#include "CompositeValue.h"

#include "Geometry.h"

#include "MultiLoader.h"
#include "Job.h"

#include "MemoryFile.h"

#include <map>

namespace xml_reader
{
	struct XMLTree;
	struct Node;
}

namespace geo
{
	class AnimationTypeDef : public ReferenceTypeDef
	{
		TYPE_DEF_BODY(AnimationTypeDef)

	public:
		TypeProperty m_colladaFile;
		TypeProperty m_hash;

		AnimationTypeDef();
		virtual ~AnimationTypeDef();

		void Construct(Value& container) const override;
	};


	class Animation : public ObjectValue, public jobs::LoadingClass
	{
	private:
		void SerializeToMF(files::MemoryFile& mf);
		void DeserializeFromMF(files::MemoryFile& mf);

		std::map<std::string, int> m_boneIndices;
		std::vector<std::string> m_boneNames;

	protected:
		virtual void LoadData(jobs::Job* done) override;

	public:
		Value m_colladaFile;
		Value m_hash;

#pragma region Collada read helpers
		
		bool m_zUp = false;

#pragma endregion

#pragma region Binary Serialized


#pragma endregion

		Animation(const ReferenceTypeDef& type);
		virtual ~Animation();

		int GetBoneIndex(const std::string& boneName) const;
	};
}