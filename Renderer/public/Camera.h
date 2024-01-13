#pragma once

#include "CompositeTypeDef.h"
#include "CompositeValue.h"

#include "MathUtils.h"

#include "MultiLoader.h"

namespace rendering::renderer
{
	class CameraTypeDef : public ReferenceTypeDef
	{
		TYPE_DEF_BODY(CameraTypeDef)

	public:
		TypeProperty m_fov;
		TypeProperty m_nearPlane;
		TypeProperty m_farPlane;

		CameraTypeDef();
		virtual ~CameraTypeDef();

		void Construct(Value& container) const override;
	};

	class Camera : public ObjectValue, public jobs::LoadingClass
	{
	private:
		bool m_isLoaded = false;

		bool m_aiming = false;
		float m_cursorRelativePos[2];
		float m_anglesCache[2];

		float m_azimuth = 90;
		float m_altitude = 0;

		math::Vector3 m_position{ 0, 0, -5 };
		math::Vector3 m_target{ 0, 0, 0 };

		Value m_window;
		Value m_cameraBuffer;

		void GetCoordinateVectors(math::Vector3& right, math::Vector3& fwd, math::Vector3& up);
		void HandleInput(double dt);

	protected:
		void LoadData(jobs::Job* done) override;

	public:

		Value m_fov;
		Value m_nearPlane;
		Value m_farPlane;

		Camera(const ReferenceTypeDef& typeDef);
		virtual ~Camera();

		void Update();
	};
}