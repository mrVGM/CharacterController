#include "Camera.h"

#include "PrimitiveTypes.h"

#include "ObjectValueContainer.h"

#include "CameraBuffer.h"
#include "DXMutableBuffer.h"

#include "Jobs.h"

#include "CoreUtils.h"

namespace
{
	BasicObjectContainer<rendering::renderer::CameraTypeDef> m_camera;
}

const rendering::renderer::CameraTypeDef& rendering::renderer::CameraTypeDef::GetTypeDef()
{
	if (!m_camera.m_object)
	{
		m_camera.m_object = new CameraTypeDef();
	}

	return *m_camera.m_object;
}

rendering::renderer::CameraTypeDef::CameraTypeDef() :
	ReferenceTypeDef(&TickUpdaterTypeDef::GetTypeDef(), "D026E773-A2D4-4D09-A174-A79B00E919DE")
{
	m_name = "Camera";
	m_category = "Renderer";
}

rendering::renderer::CameraTypeDef::~CameraTypeDef()
{
}

void rendering::renderer::CameraTypeDef::Construct(Value& container) const
{
	Camera* cam = new Camera(*this);
	container.AssignObject(cam);
}

rendering::renderer::Camera::Camera(const ReferenceTypeDef& typeDef) :
	TickUpdater(typeDef),
	m_cameraBuffer(DXMutableBufferTypeDef::GetTypeDef(), this)
{
}

rendering::renderer::Camera::~Camera()
{
}

void rendering::renderer::Camera::Load(jobs::Job* done)
{
	auto getCamBuffer = [=]() {
		return m_cameraBuffer.GetValue<DXMutableBuffer*>();
	};

	jobs::Job* loadCamBuffer = jobs::Job::CreateByLambda([=]() {
		DXMutableBuffer* camBuff = getCamBuffer();
		camBuff->Load(jobs::Job::CreateByLambda([=]() {
			m_isLoaded = true;
			jobs::RunSync(done);
		}));
	});

	jobs::Job* init = jobs::Job::CreateByLambda([=]() {
		ObjectValue* camBuffer = ObjectValueContainer::GetObjectOfType(render_pass::CameraBufferTypeDef::GetTypeDef());
		m_cameraBuffer.AssignObject(camBuffer);

		jobs::RunAsync(loadCamBuffer);
	});

	jobs::RunSync(init);
}

bool rendering::renderer::Camera::IsTicking()
{
	return m_isLoaded;
}

void rendering::renderer::Camera::Tick(double dt, jobs::Job* done)
{
	jobs::RunSync(done);
}

