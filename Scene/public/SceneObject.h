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
		SceneObjectTypeDef();
        virtual ~SceneObjectTypeDef();

        void Construct(Value& container) const override;
    };

	class SceneObject : public ObjectValue
	{
	public:
		SceneObject(const ReferenceTypeDef& typeDef);
		virtual ~SceneObject();

		void Load(jobs::Job* done);
	};
}