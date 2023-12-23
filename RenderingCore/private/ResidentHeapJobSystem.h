#pragma once

#include "CompositeTypeDef.h"
#include "CompositeValue.h"

namespace rendering
{
    class ResidentHeapJobSystemTypeDef : public ReferenceTypeDef
    {
        TYPE_DEF_BODY(ResidentHeapJobSystemTypeDef)

    public:
        ResidentHeapJobSystemTypeDef();
        virtual ~ResidentHeapJobSystemTypeDef();

        void Construct(Value& container) const override;
    };
}