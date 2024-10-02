#pragma once

#include "CompositeTypeDef.h"
#include "CompositeValue.h"

#include "MultiLoader.h"

#include "Jobs.h"

namespace scene
{
    class SceneObjectTypeDef : public ReferenceTypeDef
    {
        TYPE_DEF_BODY(SceneObjectTypeDef)

    public:
		TypeProperty m_actorList;

		SceneObjectTypeDef();
        virtual ~SceneObjectTypeDef();

        void Construct(Value& container) const override;
    };

	class SceneObject : public ObjectValue, public jobs::LoadingClass
	{
	private:
		Value m_actors;

	protected:
		void LoadData(jobs::Job done) override;

	public:
		Value m_actorList;

		SceneObject(const ReferenceTypeDef& typeDef);
		virtual ~SceneObject();

		Value& GetActors();
	};
}