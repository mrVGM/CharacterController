#pragma once

#include "CompositeTypeDef.h"

namespace app
{
	class AppEntryTypeDef : public ReferenceTypeDef
	{
		TYPE_DEF_BODY(AppEntryTypeDef)

	public:
		AppEntryTypeDef();

		void Construct(Value& container) const override;
	};	
}