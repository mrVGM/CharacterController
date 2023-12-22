#pragma once

#include "ReflectionEntryTypeDef.h"

#include "AppEntryObj.h"

namespace reflection
{
	class ReflectionEntryObj : public app::AppEntryObj
	{
	public:
		ReflectionEntryObj(const ReflectionEntryTypeDef& reflectionEntryTypeDef);
		~ReflectionEntryObj();

		virtual void Boot() override;
	};
}