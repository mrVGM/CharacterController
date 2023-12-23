#pragma once

#include "CompositeTypeDef.h"
#include "CompositeValue.h"

namespace rendering
{
    class ResidentHeapFenceTypeDef : public ReferenceTypeDef
    {
        TYPE_DEF_BODY(ResidentHeapFenceTypeDef)

    public:
		ResidentHeapFenceTypeDef();
        virtual ~ResidentHeapFenceTypeDef();

        void Construct(Value& container) const override;
    };
}