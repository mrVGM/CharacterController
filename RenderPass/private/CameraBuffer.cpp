#include "CameraBuffer.h"

#include "DXMutableBuffer.h"

namespace
{
	BasicObjectContainer<rendering::render_pass::CameraBufferTypeDef> m_cameraBuffer;
}


const rendering::render_pass::CameraBufferTypeDef& rendering::render_pass::CameraBufferTypeDef::GetTypeDef()
{
	if (!m_cameraBuffer.m_object)
	{
		m_cameraBuffer.m_object = new rendering::render_pass::CameraBufferTypeDef();
	}

	return *m_cameraBuffer.m_object;
}

rendering::render_pass::CameraBufferTypeDef::CameraBufferTypeDef() :
	ReferenceTypeDef(&DXMutableBufferTypeDef::GetTypeDef(), "CB10DF6E-4759-42EA-97E9-30D0F46BA56C")
{
	m_name = "Camera Buffer";
	m_category = "Render Pass";
}

rendering::render_pass::CameraBufferTypeDef::~CameraBufferTypeDef()
{
}

void rendering::render_pass::CameraBufferTypeDef::Construct(Value& container) const
{
	DXMutableBuffer* buff = new DXMutableBuffer(*this);
	buff->SetSizeAndStride(256, 256);

	container.AssignObject(buff);
}
