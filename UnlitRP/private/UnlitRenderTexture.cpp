#include "UnlitRenderTexture.h"

#include "Jobs.h"

#include "CoreUtils.h"

namespace
{
	BasicObjectContainer<rendering::unlit_rp::UnlitRenderTextureTypeDef> m_texture;
}

const rendering::unlit_rp::UnlitRenderTextureTypeDef& rendering::unlit_rp::UnlitRenderTextureTypeDef::GetTypeDef()
{
	if (!m_texture.m_object)
	{
		m_texture.m_object = new UnlitRenderTextureTypeDef();
	}

	return *m_texture.m_object;
}

rendering::unlit_rp::UnlitRenderTextureTypeDef::UnlitRenderTextureTypeDef() :
	ReferenceTypeDef(&DXTextureTypeDef::GetTypeDef(), "98914ACA-D1D5-4F8C-B5B5-7DD114C15F3F")
{
	m_name = "Unlit Render Texture";
	m_category = "Rendering";
}

rendering::unlit_rp::UnlitRenderTextureTypeDef::~UnlitRenderTextureTypeDef()
{
}

void rendering::unlit_rp::UnlitRenderTextureTypeDef::Construct(Value& container) const
{
	UnlitRenderTexture* tex = new UnlitRenderTexture(*this);
	container.AssignObject(tex);
}

void rendering::unlit_rp::UnlitRenderTexture::LoadData(jobs::Job* done)
{
	jobs::Job* init = jobs::Job::CreateByLambda([=]() {
		WindowObj* wnd = core::utils::GetWindow();
		UINT w = wnd->m_width.Get<int>();
		UINT h = wnd->m_height.Get<int>();

		SetDescription(DXTexture::CreateRenderTargetTextureDescription(w, h));

		DXTexture::LoadData(done);
	});

	jobs::RunSync(init);
}

rendering::unlit_rp::UnlitRenderTexture::UnlitRenderTexture(const ReferenceTypeDef& typeDef) :
	DXTexture(typeDef)
{
}
