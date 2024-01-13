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

	protected:
		void LoadData(jobs::Job* done) override;

	public:
		PlayerController(const ReferenceTypeDef& typeDef);
		virtual ~PlayerController();

		virtual void Tick(double dt) override;
		virtual void PrepareForNextTick() override;
	};
}