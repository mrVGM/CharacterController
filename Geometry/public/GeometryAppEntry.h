#pragma once

#include "CompositeTypeDef.h"
#include "AppEntryObj.h"

namespace geo
{
	class GeometryAppEntryTypeDef : public ReferenceTypeDef
	{
		TYPE_DEF_BODY(GeometryAppEntryTypeDef)

	public:
		GeometryAppEntryTypeDef();
		virtual ~GeometryAppEntryTypeDef();

		void Construct(Value& container) const override;
	};

	class GeometryAppEntryObj : public app::AppEntryObj
	{
	public:
		GeometryAppEntryObj(const ReferenceTypeDef& typeDef);
		virtual ~GeometryAppEntryObj();

		virtual void Boot() override;
	};
}