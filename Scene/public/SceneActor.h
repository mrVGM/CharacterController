#pragma once

#include "CompositeTypeDef.h"
#include "CompositeValue.h"

#include "Job.h"

namespace scene
{
	class SceneActorTypeDef : public ValueTypeDef
	{
		TYPE_DEF_BODY(SceneActorTypeDef)

	public:
		TypeProperty m_transform;
		TypeProperty m_actor;

		SceneActorTypeDef();
		virtual ~SceneActorTypeDef();

		void Construct(Value& container) const override;
	};

	class SceneActorValue : public CopyValue
	{
	public:
		Value m_transform;
		Value m_actorDef;
		
		SceneActorValue(size_t outer);
		~SceneActorValue();

		virtual void Copy(const CopyValue& src) override;
	};
}