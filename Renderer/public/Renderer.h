#pragma once

#include "CompositeTypeDef.h"
#include "CompositeValue.h"

#include "Jobs.h"

#include "d3dx12.h"

namespace rendering::renderer
{
	class RendererTypeDef : public ReferenceTypeDef
	{
		TYPE_DEF_BODY(RendererTypeDef)

	public:
		TypeProperty m_renderPasses;

		RendererTypeDef();
		virtual ~RendererTypeDef();

		void Construct(Value& container) const override;
	};

	class RendererObj : public ObjectValue
	{
	private:
		UINT64 m_frameIndex = 1;
		Value m_renderPasses;

		Value m_swapChain;
		Value m_commandQueue;
		Value m_renderFence;

		Value m_scene;
		Value m_camera;
	public:
		Value m_renderPassesDefs;

		RendererObj(const ReferenceTypeDef& typeDef);
		virtual ~RendererObj();

		void Load(jobs::Job done);

		Value& GetScene();
		void RenderFrame();
	};

	void Boot();
}