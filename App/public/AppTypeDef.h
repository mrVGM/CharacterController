#pragma once

#include "CompositeValue.h"

namespace app
{
	class AppTypeDef : public ReferenceTypeDef
	{
		TYPE_DEF_BODY(AppTypeDef)

	public:
		TypeProperty m_appEntry;

		AppTypeDef();

		void Construct(Value& container) const override;
	};
}