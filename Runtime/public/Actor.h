#pragma once

#include "CompositeTypeDef.h"
#include "CompositeValue.h"

#include "Mesh.h"
#include "Material.h"

#include "MultiLoader.h"
#include "Jobs.h"

#include "d3dx12.h"

#include<wrl.h>
#include <map>
#include <list>

namespace runtime
{
	class ActorTypeDef : public ReferenceTypeDef
	{
		TYPE_DEF_BODY(ActorTypeDef)

	public:
		ActorTypeDef();
		virtual ~ActorTypeDef();

		void Construct(Value& container) const override;
	};

	class Actor : public ObjectValue, public jobs::LoadingClass
	{
	protected:

	public:
		math::Transform m_curTransform;

		Actor(const ReferenceTypeDef& typeDef);
		virtual ~Actor();
		
		virtual void Tick(double dt);
		virtual void PrepareForNextTick();
	};
}