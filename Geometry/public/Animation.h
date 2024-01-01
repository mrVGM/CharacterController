#pragma once

#include "CompositeTypeDef.h"
#include "CompositeValue.h"

#include "Geometry.h"
#include "Skeleton.h"

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
	public:
		struct KeyFrame
		{
			float m_time;
			math::Matrix m_transform;
		};

		struct AnimChannel
		{
			std::string m_name;
			std::vector<KeyFrame> m_keyFrames;
		};

	private:
		void SerializeToMF(files::MemoryFile& mf);
		void DeserializeFromMF(files::MemoryFile& mf);

		std::map<std::string, const AnimChannel*> m_channelMap;

	protected:
		virtual void LoadData(jobs::Job* done) override;

	public:
		Value m_colladaFile;
		Value m_hash;

#pragma region Collada read helpers
		
		bool m_zUp = false;

#pragma endregion

#pragma region Binary Serialized

		std::vector<AnimChannel> m_animation;

#pragma endregion

		Animation(const ReferenceTypeDef& type);
		virtual ~Animation();

		double GetLength() const;
		const AnimChannel* GetAnimChannel(const std::string& name) const;
		const math::Matrix& SampleChannel(double time, const AnimChannel& channel) const;
	};
}