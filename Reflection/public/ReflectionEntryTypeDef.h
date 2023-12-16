#pragma once

#include "CompositeTypeDef.h"

namespace reflection
{
	class ReflectionEntryTypeDef : public ReferenceTypeDef
	{
	public:
		static const ReflectionEntryTypeDef& GetReflectionEntryTypeDef();
		ReflectionEntryTypeDef();

		void Construct(Value& container) const override;
	};	
}