#pragma once

#include "ReflectionEntryTypeDef.h"

#include "AppEntryObj.h"

namespace reflection
{
	class ReflectionEntryObj : public app::AppEntryObj
	{
	public:
		ReflectionEntryObj(const ReflectionEntryTypeDef& reflectionEntryTypeDef, const CompositeValue* outer);
		virtual void Boot() override;
	};
}