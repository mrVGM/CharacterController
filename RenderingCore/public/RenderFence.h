#pragma once

#include "CompositeTypeDef.h"
#include "CompositeValue.h"

namespace rendering
{
    class RenderFenceTypeDef : public ReferenceTypeDef
    {
        TYPE_DEF_BODY(RenderFenceTypeDef)

    public:
        RenderFenceTypeDef();
        virtual ~RenderFenceTypeDef();

        void Construct(Value& container) const override;
    };
}