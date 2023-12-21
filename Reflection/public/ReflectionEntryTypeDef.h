#pragma once

#include "CompositeTypeDef.h"

namespace reflection
{
	class ReflectionEntryTypeDef : public ReferenceTypeDef
	{
		TYPE_DEF_BODY(ReflectionEntryTypeDef)

	public:
		ReflectionEntryTypeDef();
		virtual ~ReflectionEntryTypeDef();

		void Construct(Value& container) const override;
	};	
}