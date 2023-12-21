#pragma once

#include "CompositeTypeDef.h"

namespace rendering::core
{
	class RenderingEntryTypeDef : public ReferenceTypeDef
	{
		TYPE_DEF_BODY(RenderingEntryTypeDef)

	public:
		RenderingEntryTypeDef();
		virtual ~RenderingEntryTypeDef();

		void Construct(Value& container) const override;
	};	
}