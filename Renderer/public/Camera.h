#pragma once

#include "CompositeTypeDef.h"
#include "CompositeValue.h"

#include "MathUtils.h"

#include "TickUpdater.h"

namespace rendering::renderer
{
	class CameraTypeDef : public ReferenceTypeDef
	{
		TYPE_DEF_BODY(CameraTypeDef)

	public:
		CameraTypeDef();
		virtual ~CameraTypeDef();

		void Construct(Value& container) const override;
	};

	class Camera : public TickUpdater
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

	public:
		Camera(const ReferenceTypeDef& typeDef);
		virtual ~Camera();

		void Load(jobs::Job* done);

		bool IsTicking() override;
		void Tick(double dt, jobs::Job* done) override;
	};
}