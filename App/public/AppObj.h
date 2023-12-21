#pragma once

#include "AppTypeDef.h"

namespace app
{
	class AppObj : public ObjectValue
	{
	public:
		Value m_appEntry;

		AppObj(const AppTypeDef& appTypedef);
		virtual ~AppObj();
	};
}