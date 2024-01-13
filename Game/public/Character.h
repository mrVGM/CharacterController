#pragma once

#include "MeshActor.h"

#include "MathUtils.h"

namespace game
{
	class CharacterTypeDef : public ReferenceTypeDef
	{
		TYPE_DEF_BODY(CharacterTypeDef)

	public:
		CharacterTypeDef();
		virtual ~CharacterTypeDef();

		void Construct(Value& container) const override;
	};

	class Character : public runtime::MeshActor
	{
	public:
		math::Vector3 m_velocity{ 0, 0, 0 };

		Character(const ReferenceTypeDef& typeDef);
		virtual ~Character();
	};
}