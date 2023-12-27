#pragma once

#include "DXTexture.h"

namespace rendering
{
    class DXDepthStencilTextureTypeDef : public ReferenceTypeDef
    {
        TYPE_DEF_BODY(DXDepthStencilTextureTypeDef)

    public:
		DXDepthStencilTextureTypeDef();
        virtual ~DXDepthStencilTextureTypeDef();

        void Construct(Value& container) const override;
    };
}