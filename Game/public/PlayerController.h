#pragma once

#include "Actor.h"

namespace game
{
	class PlayerControllerTypeDef : public ReferenceTypeDef
	{
		TYPE_DEF_BODY(PlayerControllerTypeDef)

	public:
		PlayerControllerTypeDef();
		virtual ~PlayerControllerTypeDef();

		void Construct(Value& container) const override;
	};

	class PlayerController : public runtime::Actor
	{
	private:
		Value m_camera;
		Value m_scene;
		Value m_character;

		bool m_justPossesed = true;

		void FreeMove(double dt);
		void ControllCharacter(double dt);

		void HandleCharMove(float dt, const math::Vector3& velocity);

	protected:
		void LoadData(jobs::Job* done) override;

	public:
		PlayerController(const ReferenceTypeDef& typeDef);
		virtual ~PlayerController();

		virtual void Tick(double dt) override;
		virtual void PrepareForNextTick() override;
	};
}