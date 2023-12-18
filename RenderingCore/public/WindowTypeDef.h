#pragma once

#include "CompositeTypeDef.h"

namespace rendering
{
	class WindowTypeDef : public ReferenceTypeDef
	{
		TYPE_DEF_BODY(WindowTypeDef)

	public:
		TypeProperty m_width;
		TypeProperty m_height;
		TypeProperty m_windowUpdateJobSystem;

		WindowTypeDef();
		virtual void Construct(Value& container) const override;
	};
}