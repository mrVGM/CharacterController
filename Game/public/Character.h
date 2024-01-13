#pragma once

#include "MeshActor.h"

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
		Character(const ReferenceTypeDef& typeDef);
		virtual ~Character();
	};
}