#pragma once

#include "CompositeTypeDef.h"
#include "CompositeValue.h"

namespace app
{
	class AppEntryTypeDef : public ReferenceTypeDef
	{
		TYPE_DEF_BODY(AppEntryTypeDef)

	public:
		AppEntryTypeDef();
		virtual ~AppEntryTypeDef();

		void Construct(Value& container) const override;
	};

	class AppEntryObj : public ObjectValue
	{
	public:
		AppEntryObj(const ReferenceTypeDef& appEntryTypeDef);
		virtual ~AppEntryObj();
		virtual void Boot();
	};
}