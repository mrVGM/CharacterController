#pragma once

#include "AppEntryTypeDef.h"
#include "CompositeValue.h"

namespace app
{
	class AppEntryObj : public ObjectValue
	{
	public:
		AppEntryObj(const ReferenceTypeDef& appEntryTypeDef);
		virtual ~AppEntryObj();
		virtual void Boot();
	};
}