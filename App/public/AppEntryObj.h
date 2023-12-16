#pragma once

#include "AppEntryTypeDef.h"
#include "CompositeValue.h"

namespace app
{
	class AppEntryObj : public ObjectValue
	{
	public:
		AppEntryObj(const ReferenceTypeDef& appEntryTypeDef, const CompositeValue* outer);
		virtual void Boot();
	};
}