#include "Camera.h"

#include "PrimitiveTypes.h"

#include "ObjectValueContainer.h"

#include "CameraBuffer.h"
#include "DXMutableBuffer.h"

#include "Jobs.h"

#include "CoreUtils.h"

#include <corecrt_math_defines.h>


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
	ReferenceTypeDef(&TickUpdaterTypeDef::GetTypeDef(), "D026E773-A2D4-4D09-A174-A79B00E919DE"),
	m_fov("693B0DAC-75B2-42B0-AABF-1A447547B720", FloatTypeDef::GetTypeDef()),
	m_nearPlane("E42C1E0C-C69B-4F86-8EB2-4B5D3550249C", FloatTypeDef::GetTypeDef()),
	m_farPlane("90F3E332-D507-4BCB-BCBE-E74E89F235F6", FloatTypeDef::GetTypeDef())
{
	{
		m_fov.m_name = "FOV";
		m_fov.m_category = "Setup";
		m_fov.m_getValue = [](CompositeValue* obj) -> Value& {
			Camera* cam = static_cast<Camera*>(obj);
			return cam->m_fov;
		};
		m_properties[m_fov.GetId()] = &m_fov;
	}

	{
		m_nearPlane.m_name = "Near Plane";
		m_nearPlane.m_category = "Setup";
		m_nearPlane.m_getValue = [](CompositeValue* obj) -> Value& {
			Camera* cam = static_cast<Camera*>(obj);
			return cam->m_nearPlane;
		};
		m_properties[m_nearPlane.GetId()] = &m_nearPlane;
	}

	{
		m_farPlane.m_name = "Far Plane";
		m_farPlane.m_category = "Setup";
		m_farPlane.m_getValue = [](CompositeValue* obj) -> Value& {
			Camera* cam = static_cast<Camera*>(obj);
			return cam->m_farPlane;
		};
		m_properties[m_farPlane.GetId()] = &m_farPlane;
	}

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

void rendering::renderer::Camera::GetCoordinateVectors(math::Vector3& right, math::Vector3& fwd, math::Vector3& up)
{
	using namespace math;
	fwd = m_target + -1 * m_position;

	up = Vector3{ 0, 1, 0 };
	right = up ^ fwd;
	up = fwd ^ right;

	fwd = fwd.Normalize();
	right = right.Normalize();
	up = up.Normalize();
}

void rendering::renderer::Camera::HandleInput(double dt)
{
	using namespace math;

	WindowObj* wnd = m_window.GetValue<WindowObj*>();
	InputInfo inputInfo = wnd->GetInputInfo();

	float right = 0;
	float forward = 0;
	float aimRight = 0;
	float aimUp = 0;

	for (std::set<WPARAM>::const_iterator it = inputInfo.m_keysDown.begin(); it != inputInfo.m_keysDown.end(); ++it) {
		WPARAM x = *it;
		if (x == 65) {
			right = -1;
		}
		if (x == 68) {
			right = 1;
		}

		if (x == 87) {
			forward = 1;
		}
		if (x == 83) {
			forward = -1;
		}

		if (x == 37) {
			aimRight = 1;
		}
		if (x == 39) {
			aimRight = -1;
		}
		if (x == 38) {
			aimUp = 1;
		}
		if (x == 40) {
			aimUp = -1;
		}
	}

	RECT rect;
	GetWindowRect(wnd->m_hwnd, &rect);

	if (inputInfo.m_rightMouseButtonDown && !m_aiming) {
		m_cursorRelativePos[0] = inputInfo.m_mouseMovement[0];
		m_cursorRelativePos[1] = inputInfo.m_mouseMovement[1];
		m_anglesCache[0] = m_azimuth;
		m_anglesCache[1] = m_altitude;

		ClipCursor(&rect);
		ShowCursor(false);
	}

	if (!inputInfo.m_rightMouseButtonDown && m_aiming) {
		ClipCursor(nullptr);
		ShowCursor(true);
	}


	double m_mouseAngleSpeed = 0.1;
	double m_angleSpeed = 80;
	double m_moveSpeed = 0.3;

	m_aiming = inputInfo.m_rightMouseButtonDown;
	if (m_aiming) {
		SetCursorPos((rect.left + rect.right) / 2, (rect.top + rect.bottom) / 2);

		m_azimuth = -m_mouseAngleSpeed * (inputInfo.m_mouseMovement[0] - m_cursorRelativePos[0]) + m_anglesCache[0];
		while (m_azimuth >= 360) {
			m_azimuth -= 360;
		}
		while (m_azimuth < 0) {
			m_azimuth += 360;
		}

		m_altitude = -m_mouseAngleSpeed * (inputInfo.m_mouseMovement[1] - m_cursorRelativePos[1]) + m_anglesCache[1];
		if (m_altitude > 80) {
			m_altitude = 80;
		}

		if (m_altitude < -80) {
			m_altitude = -80;
		}
	}

	m_azimuth += dt * m_angleSpeed * aimRight;
	while (m_azimuth >= 360) {
		m_azimuth -= 360;
	}
	while (m_azimuth < 0) {
		m_azimuth += 360;
	}

	m_altitude += dt * m_angleSpeed * aimUp;
	if (m_altitude > 80) {
		m_altitude = 80;
	}

	if (m_altitude < -80) {
		m_altitude = -80;
	}


	float azimuth = M_PI * m_azimuth / 180.0;
	float altitude = M_PI * m_altitude / 180.0;

	Vector3 fwdVector = { cos(azimuth) * cos(altitude), sin(altitude), sin(azimuth) * cos(altitude) };
	Vector3 rightVector = Vector3{ 0, 1, 0 } ^ fwdVector;
	rightVector = rightVector.Normalize();

	Vector3 moveVector{ right, 0, forward };
	moveVector = moveVector.Normalize();
	moveVector = m_moveSpeed * moveVector;
	moveVector = moveVector.m_coefs[0] * rightVector + moveVector.m_coefs[2] * fwdVector;

	m_position = m_position + moveVector;
	m_target = m_position + fwdVector;
}

rendering::renderer::Camera::Camera(const ReferenceTypeDef& typeDef) :
	TickUpdater(typeDef),
	m_cameraBuffer(DXMutableBufferTypeDef::GetTypeDef(), this),
	m_window(WindowTypeDef::GetTypeDef(), this),

	m_fov(CameraTypeDef::GetTypeDef().m_fov.GetType(), this),
	m_nearPlane(CameraTypeDef::GetTypeDef().m_nearPlane.GetType(), this),
	m_farPlane(CameraTypeDef::GetTypeDef().m_farPlane.GetType(), this)
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
		m_window.AssignObject(core::utils::GetWindow());

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
	using namespace math;

	WindowObj* wnd = m_window.GetValue<WindowObj*>();

	HandleInput(dt);
	Vector3 right, fwd, up;
	GetCoordinateVectors(right, fwd, up);

	const float fov = m_fov.Get<float>();
	const float aspect = static_cast<float>(wnd->m_width.Get<int>()) / static_cast<float>(wnd->m_height.Get<int>());
	const float farPlane = m_farPlane.Get<float>();
	const float nearPlane = m_nearPlane.Get<float>();

	float fovRad = M_PI * fov / 180;

	float h = tan(fovRad / 2);
	float w = aspect * h;

	Matrix translate = Matrix::GetIdentityMatrix();

	translate.GetCoef(0, 3) = -m_position.m_coefs[0];
	translate.GetCoef(1, 3) = -m_position.m_coefs[1];
	translate.GetCoef(2, 3) = -m_position.m_coefs[2];

	Matrix view = {
		{
			right.m_coefs[0],	right.m_coefs[1],	right.m_coefs[2],	0,
			up.m_coefs[0],		up.m_coefs[1],		up.m_coefs[2],		0,
			fwd.m_coefs[0],		fwd.m_coefs[1],		fwd.m_coefs[2],		0,
			0,					0,					0,					1
		}
	};

	Matrix project = {
		{
			1 / w, 0, 0, 0,
			0, 1 / h, 0, 0,
			0, 0, farPlane / (farPlane - nearPlane), -farPlane * nearPlane / (farPlane - nearPlane),
			0, 0, 1, 0
		}
	};

	Matrix camProjection = project * view * translate;
	camProjection = camProjection.Transpose();

	DXMutableBuffer* camBuff = m_cameraBuffer.GetValue<DXMutableBuffer*>();
	DXBuffer* uploadBuff = camBuff->m_uploadBuffer.GetValue<DXBuffer*>();
	Matrix* data = static_cast<Matrix*>(uploadBuff->Map());
	*data = camProjection;
	uploadBuff->Unmap();

	camBuff->SetDirty(true);

	jobs::RunSync(done);
}

