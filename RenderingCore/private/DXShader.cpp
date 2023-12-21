#include "DXShader.h"

#include "PrimitiveTypes.h"

namespace
{
	BasicObjectContainer<rendering::DXShaderTypeDef> m_shaderTypeDef;
	BasicObjectContainer<rendering::DXVertexShaderTypeDef> m_vertexShaderTypeDef;
	BasicObjectContainer<rendering::DXPixelShaderTypeDef> m_pixelShaderTypeDef;
}

rendering::DXShaderTypeDef::DXShaderTypeDef() :
	ReferenceTypeDef(&ReferenceTypeDef::GetTypeDef(), "7D55F6EB-BDB6-4199-83E8-5069BE1EB80F"),
	m_shaderName("130EF692-9CC6-4C8C-89B2-49A5D2F8EED2", StringTypeDef::GetTypeDef())
{
	{
		m_shaderName.m_name = "Shader Name";
		m_shaderName.m_category = "Setup";
		m_shaderName.m_getValue = [](CompositeValue* obj) -> Value& {
			DXShader* shader = static_cast<DXShader*>(obj);
			return shader->m_name;
		};
		m_properties[m_shaderName.GetId()] = &m_shaderName;
	}

	m_name = "Shader";
	m_category = "Rendering";
}

const rendering::DXShaderTypeDef& rendering::DXShaderTypeDef::GetTypeDef()
{
	if (!m_shaderTypeDef.m_object)
	{
		m_shaderTypeDef.m_object = new DXShaderTypeDef();
	}

	return *m_shaderTypeDef.m_object;
}

rendering::DXVertexShaderTypeDef::DXVertexShaderTypeDef() :
	ReferenceTypeDef(&rendering::DXShaderTypeDef::GetTypeDef(), "F2A064DD-D5B3-44B4-94B5-AFC68840828D")
{
}

void rendering::DXVertexShaderTypeDef::Construct(Value& container) const
{
	DXShader* shader = new DXShader(GetTypeDef());
	container.AssignObject(shader);
}

const rendering::DXVertexShaderTypeDef& rendering::DXVertexShaderTypeDef::GetTypeDef()
{
	if (!m_vertexShaderTypeDef.m_object)
	{
		m_vertexShaderTypeDef.m_object = new DXVertexShaderTypeDef();
	}

	return *m_vertexShaderTypeDef.m_object;
}

rendering::DXPixelShaderTypeDef::DXPixelShaderTypeDef() :
	ReferenceTypeDef(&rendering::DXShaderTypeDef::GetTypeDef(), "3823F212-4D44-453B-9DB9-F270A15FD135")
{
}

void rendering::DXPixelShaderTypeDef::Construct(Value& container) const
{
	DXShader* shader = new DXShader(GetTypeDef());
	container.AssignObject(shader);
}

const rendering::DXPixelShaderTypeDef& rendering::DXPixelShaderTypeDef::GetTypeDef()
{
	if (!m_pixelShaderTypeDef.m_object)
	{
		m_pixelShaderTypeDef.m_object = new DXPixelShaderTypeDef();
	}

	return *m_pixelShaderTypeDef.m_object;
}

rendering::DXShader::DXShader(const ReferenceTypeDef& type) :
	ObjectValue(type),
	m_name(StringTypeDef::GetTypeDef(), this)
{
}

ID3DBlob* rendering::DXShader::GetCompiledShader() const
{
	return m_shader.Get();
}
