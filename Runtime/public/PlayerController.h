#pragma once

#include "TickUpdater.h"

#include<wrl.h>
#include <map>
#include <list>

namespace runtime
{
	class PlayerControllerTypeDef : public ReferenceTypeDef
	{
		TYPE_DEF_BODY(PlayerControllerTypeDef)

	public:
		PlayerControllerTypeDef();
		virtual ~PlayerControllerTypeDef();

		void Construct(Value& container) const override;
	};

	class PlayerController : public TickUpdater
	{
	public:
		PlayerController(const ReferenceTypeDef& typeDef);
		virtual ~PlayerController();

		virtual bool IsTicking() override;
		virtual void Tick(double dt, jobs::Job* done) override;
	};
}