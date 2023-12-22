#pragma once

#include "CompositeTypeDef.h"

#include "AppEntryObj.h"

namespace rendering::core
{
	class RenderingEntryObj : public app::AppEntryObj
	{
	public:
		RenderingEntryObj(const ReferenceTypeDef& typeDef);
		~RenderingEntryObj();

		virtual void Boot() override;
	};
}