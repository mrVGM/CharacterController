#include "DXDepthStencilTexture.h"

#include "Jobs.h"

#include "RenderWindow.h"

#include "ObjectValueContainer.h"

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
	DXDepthStencilTexture* tex = new DXDepthStencilTexture(DXDepthStencilTextureTypeDef::GetTypeDef());
	container.AssignObject(tex);
}

void rendering::DXDepthStencilTexture::LoadData(jobs::Job* done)
{
	jobs::Job* init = jobs::Job::CreateByLambda([=]() {
		WindowObj* wnd = static_cast<WindowObj*>(ObjectValueContainer::GetObjectOfType(WindowTypeDef::GetTypeDef()));
		UINT w = wnd->m_width.Get<int>();
		UINT h = wnd->m_height.Get<int>();

		SetDescription(DXTexture::CreateDepthStencilTextureDescription(w, h));

		DXTexture::LoadData(done);
	});

	jobs::RunSync(init);
}

rendering::DXDepthStencilTexture::DXDepthStencilTexture(const ReferenceTypeDef& typeDef) :
	DXTexture(typeDef)
{
}
