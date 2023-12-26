#pragma once

#include "AppEntry.h"

#include "Job.h"

#include "CompositeTypeDef.h"

#include "d3dx12.h"

namespace rendering
{
	class RendererAppEntryTypeDef : public ReferenceTypeDef
	{
		TYPE_DEF_BODY(RendererAppEntryTypeDef)

	public:
		RendererAppEntryTypeDef();
		virtual ~RendererAppEntryTypeDef();

		void Construct(Value& container) const override;
	};

	class RendererAppEntryObj : public app::AppEntryObj
	{
	private:
		Value m_renderer;
		Value m_copyBuffers;

		int m_copyListsSize = 0;
		ID3D12CommandList** m_copyCommandLists = nullptr;

		void Tick();
		void UpdateMutableBuffers(jobs::Job* done);

	public:
		RendererAppEntryObj(const ReferenceTypeDef& typeDef);
		virtual ~RendererAppEntryObj();

		virtual void Boot() override;
	};
}