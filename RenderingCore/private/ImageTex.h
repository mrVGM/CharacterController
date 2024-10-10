#pragma once

#include "CompositeTypeDef.h"
#include "DXTexture.h"

#include "Jobs.h"

#include <wrl.h>
#include <wincodec.h>
#include <string>
#include <map>

namespace rendering
{
	class DXTexture;
}

namespace rendering
{
	class ImageTexTypeDef : public ReferenceTypeDef
    {
        TYPE_DEF_BODY(ImageTexTypeDef)

    public:
		TypeProperty m_file;

		ImageTexTypeDef();
        virtual ~ImageTexTypeDef();

        void Construct(Value& container) const override;
    };

	class ImageTex : public DXTexture
	{
	private:
		friend class ImageTexTypeDef;
		Value m_file;

		Microsoft::WRL::ComPtr<IWICImagingFactory> m_factory;
		std::map<std::string, DXTexture*> m_imagesRepo;

	protected:
		virtual void LoadData(jobs::Job done) override;

	public:

		ImageTex();
		virtual ~ImageTex();
	};
}