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

	class Character;

	struct JumpComponent
	{
		bool m_duringJump = false;
		double m_jumpTime = 0;
		Character& m_character;

	public:
		JumpComponent(Character& character);

		void StartJump();
		void UpdateJump(double dt);
	};

	class Character : public runtime::MeshActor
	{
	public:
		JumpComponent m_jumpComponent;
		math::Vector3 m_velocity{ 0, 0, 0 };

		Character(const ReferenceTypeDef& typeDef);
		virtual ~Character();
	};
}