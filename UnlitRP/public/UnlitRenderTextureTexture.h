#pragma once

#include "DXTexture.h"

namespace rendering::unlit_rp
{
    class UnlitRenderTextureTypeDef : public ReferenceTypeDef
    {
        TYPE_DEF_BODY(UnlitRenderTextureTypeDef)

    public:
        UnlitRenderTextureTypeDef();
        virtual ~UnlitRenderTextureTypeDef();

        void Construct(Value& container) const override;
    };

    class UnlitRenderTexture : public DXTexture
    {
    protected:
        virtual void LoadData(jobs::Job* done);

    public:
        UnlitRenderTexture(const ReferenceTypeDef& typeDef);
    };
}