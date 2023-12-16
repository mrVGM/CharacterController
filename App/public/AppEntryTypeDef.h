#pragma once

#include "CompositeTypeDef.h"

namespace app
{
	class AppEntryTypeDef : public ReferenceTypeDef
	{
	public:
		static const AppEntryTypeDef& GetAppEntryTypeDef();
		AppEntryTypeDef();

		void Construct(Value& container) const override;
	};	
}