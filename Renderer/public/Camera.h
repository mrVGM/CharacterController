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
		Value m_cameraBuffer;

	public:
		Camera(const ReferenceTypeDef& typeDef);
		virtual ~Camera();

		void Load(jobs::Job* done);

		bool IsTicking() override;
		void Tick(double dt, jobs::Job* done) override;
	};
}