#pragma once

#include "CompositeValue.h"

namespace app
{
	class AppTypeDef : public ReferenceTypeDef
	{
	public:
		static const AppTypeDef& GetAppTypeDef();

		TypeProperty m_appEntry;

		AppTypeDef();

		void Construct(Value& container) const override;
	};
}