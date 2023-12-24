#pragma once

#include "AppEntry.h"

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
	private:
		Value m_renderer;

	public:
		RendererAppEntryObj(const ReferenceTypeDef& typeDef);
		virtual ~RendererAppEntryObj();

		virtual void Boot() override;
	};
}