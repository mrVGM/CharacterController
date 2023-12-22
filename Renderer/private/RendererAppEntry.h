#pragma once

#include "AppEntryObj.h"

#include "CompositeTypeDef.h"

namespace rendering
{
	class RendererAppEntryTypeDef : public ReferenceTypeDef
	{
		TYPE_DEF_BODY(RendererAppEntryTypeDef)

	public:
		RendererAppEntryTypeDef();
		virtual ~RendererAppEntryTypeDef();

		void Construct(Value& container) const override;
	};

	class RendererAppEntryObj : public app::AppEntryObj
	{
	public:
		RendererAppEntryObj(const ReferenceTypeDef& typeDef);
		virtual void Boot() override;
	};
}