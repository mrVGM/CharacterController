#pragma once

#include "CompositeTypeDef.h"
#include "CompositeValue.h"

#include "Job.h"

namespace runtime
{
	class TickUpdaterTypeDef : public ReferenceTypeDef
	{
		TYPE_DEF_BODY(TickUpdaterTypeDef)

	public:
		TickUpdaterTypeDef();
		virtual ~TickUpdaterTypeDef();

		void Construct(Value& container) const override;
	};

	class TickUpdater : public ObjectValue
	{
	public:
		TickUpdater(const ReferenceTypeDef& typeDef);
		virtual ~TickUpdater();

		virtual bool IsTicking() = 0;
		virtual void Tick(double dt, jobs::Job* done) = 0;
	};
}