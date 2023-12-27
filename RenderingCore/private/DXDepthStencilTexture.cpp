#include "DXDepthStencilTexture.h"

#include "CoreUtils.h"

namespace
{
	BasicObjectContainer<rendering::DXDepthStencilTextureTypeDef> m_texture;
}

const rendering::DXDepthStencilTextureTypeDef& rendering::DXDepthStencilTextureTypeDef::GetTypeDef()
{
	if (!m_texture.m_object)
	{
		m_texture.m_object = new DXDepthStencilTextureTypeDef();
	}

	return *m_texture.m_object;
}

rendering::DXDepthStencilTextureTypeDef::DXDepthStencilTextureTypeDef() :
	ReferenceTypeDef(&DXTextureTypeDef::GetTypeDef(), "E322B05E-00DF-4D51-8F82-7044ACF552DB")
{
	m_name = "Depth Stencil Texture";
	m_category = "Rendering";
}

rendering::DXDepthStencilTextureTypeDef::~DXDepthStencilTextureTypeDef()
{
}

void rendering::DXDepthStencilTextureTypeDef::Construct(Value& container) const
{
	WindowObj* wnd = core::utils::GetWindow();
	UINT w = wnd->m_width.Get<int>();
	UINT h = wnd->m_height.Get<int>();

	DXTexture* tex = DXTexture::CreateDepthStencilTexture(DXDepthStencilTextureTypeDef::GetTypeDef(), w, h);
	container.AssignObject(tex);
}
