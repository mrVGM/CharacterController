#pragma once

#include "AppEntry.h"

#include "Job.h"

#include "CompositeTypeDef.h"

#include "d3dx12.h"

#include <chrono>

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
		bool m_firstTick = true;

		Value m_renderer;
		Value m_copyBuffers;
		Value m_window;

		int m_copyListsSize = 0;
		ID3D12CommandList** m_copyCommandLists = nullptr;

		bool m_startedTicking = false;
		std::chrono::system_clock::time_point m_lastTick;

		void Tick();
		void UpdateMutableBuffers(jobs::Job* done);
		void RunTickUpdaters(double dt, jobs::Job* done);

		double TimeStamp();
	public:
		RendererAppEntryObj(const ReferenceTypeDef& typeDef);
		virtual ~RendererAppEntryObj();

		virtual void Boot() override;
	};
}