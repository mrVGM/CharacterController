#pragma once

#include "CompositeTypeDef.h"
#include "CompositeValue.h"

#include "Job.h"

namespace scene
{
    class SceneObjectTypeDef : public ReferenceTypeDef
    {
        TYPE_DEF_BODY(SceneObjectTypeDef)

    public:
		TypeProperty m_actorList;
		TypeProperty m_sceneActorList;

		SceneObjectTypeDef();
        virtual ~SceneObjectTypeDef();

        void Construct(Value& container) const override;
    };

	class SceneObject : public ObjectValue
	{
	private:
		Value m_actors;

	public:
		Value m_actorDefList;
		Value m_sceneActorList;

		SceneObject(const ReferenceTypeDef& typeDef);
		virtual ~SceneObject();

		void Load(jobs::Job* done);
		Value& GetActors();
	};
}