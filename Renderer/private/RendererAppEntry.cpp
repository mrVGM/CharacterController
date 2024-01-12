#include "RendererAppEntry.h"

#include "AppEntry.h"

#include "Jobs.h"
#include "ObjectValueContainer.h"
#include "Renderer.h"

#include "DXMutableBuffer.h"

#include "TickUpdater.h"

#include "Input.h"

#include "RenderWindow.h"

#include "DXCopyBuffers.h"

namespace
{
	BasicObjectContainer<rendering::RendererAppEntryTypeDef> m_rendererAppEntry;
}

const rendering::RendererAppEntryTypeDef& rendering::RendererAppEntryTypeDef::GetTypeDef()
{
	if (!m_rendererAppEntry.m_object)
	{
		m_rendererAppEntry.m_object = new rendering::RendererAppEntryTypeDef();
	}

	return *m_rendererAppEntry.m_object;
}

rendering::RendererAppEntryTypeDef::RendererAppEntryTypeDef() :
	ReferenceTypeDef(&app::AppEntryTypeDef::GetTypeDef(), "62CBB02A-9574-4F11-B468-F14334EC6B16")
{
	m_name = "Renderer App Entry";
	m_category = "Renderer";
}

rendering::RendererAppEntryTypeDef::~RendererAppEntryTypeDef()
{
}

void rendering::RendererAppEntryTypeDef::Construct(Value& container) const
{
	RendererAppEntryObj* entry = new RendererAppEntryObj(*this);
	container.AssignObject(entry);
}

void rendering::RendererAppEntryObj::Tick()
{
	static int m_running;
	m_running = 2;

	rendering::InputInfo inputInfo;
	WindowObj* wnd = m_window.GetValue<WindowObj*>();
	wnd->GetInputInfo(inputInfo);
	if (m_firstTick)
	{
		inputInfo.m_mouseAxis[0] = 0;
		inputInfo.m_mouseAxis[1] = 0;
	}
	m_firstTick = false;

	runtime::m_input.m_mouseAxis[0] = inputInfo.m_mouseAxis[0];
	runtime::m_input.m_mouseAxis[1] = inputInfo.m_mouseAxis[1];
	runtime::m_input.m_keysDown = inputInfo.m_keysDown;
	runtime::m_input.m_lbmDown = inputInfo.m_leftMouseButtonDown;
	runtime::m_input.m_rbmDown = inputInfo.m_rightMouseButtonDown;

	auto jobDone = [=]() {
		--m_running;
		if (m_running > 0)
		{
			return;
		}

		jobs::Job* updateMutable = jobs::Job::CreateByLambda([=]() {
			UpdateMutableBuffers(jobs::Job::CreateByLambda([=]() {
				jobs::Job* nextTick = jobs::Job::CreateByLambda([=]() {
					m_startedTicking = true;
					Tick();
				});
				jobs::RunAsync(nextTick);
			}));
		});

		jobs::RunAsync(updateMutable);
	};

	renderer::RendererObj* rend = m_renderer.GetValue<renderer::RendererObj*>();
	jobs::RunAsync(jobs::Job::CreateByLambda([=]() {
		rend->RenderFrame();
		jobs::RunSync(jobs::Job::CreateByLambda(jobDone));
	}));

	jobs::RunAsync(jobs::Job::CreateByLambda([=]() {
		double dt = TimeStamp();
		RunTickUpdaters(dt, jobs::Job::CreateByLambda(jobDone));
	}));
}

void rendering::RendererAppEntryObj::UpdateMutableBuffers(jobs::Job* done)
{
	jobs::Job* getBuffers = jobs::Job::CreateByLambda([=]() {
		std::list<ObjectValue*> mutableBuffers;
		ObjectValueContainer& container = ObjectValueContainer::GetContainer();
		container.GetObjectsOfType(DXMutableBufferTypeDef::GetTypeDef(), mutableBuffers);

		if (m_copyListsSize < mutableBuffers.size())
		{
			m_copyListsSize = mutableBuffers.size();
			if (m_copyCommandLists)
			{
				delete[] m_copyCommandLists;
			}

			m_copyCommandLists = new ID3D12CommandList * [m_copyListsSize];
		}

		bool anyBuffer = false;
		int numLists = 0;
		for (auto it = mutableBuffers.begin(); it != mutableBuffers.end(); ++it)
		{
			DXMutableBuffer* cur = static_cast<DXMutableBuffer*>(*it);
			if (!cur->IsDirty())
			{
				continue;
			}

			anyBuffer = true;
			cur->SetDirty(false);

			m_copyCommandLists[numLists++] = cur->GetCopyCommandList();
		}

		if (!anyBuffer)
		{
			jobs::RunSync(done);
			return;
		}

		jobs::RunAsync(jobs::Job::CreateByLambda([=]() {
			m_copyBuffers.GetValue<DXCopyBuffers*>()->Execute(m_copyCommandLists, numLists, done);
		}));
	});

	jobs::RunSync(getBuffers);
}

void rendering::RendererAppEntryObj::RunTickUpdaters(double dt, jobs::Job* done)
{
	using namespace renderer;

	static std::list<ObjectValue*> updaters;
	static int updatersRunning;

	updaters.clear();
	updatersRunning = 0;

	auto updaterDone = [=]() {
		--updatersRunning;
		if (updatersRunning > 0)
		{
			return;
		}

		jobs::RunSync(done);
	};

	auto startUpdater = [=](runtime::TickUpdater* updater) {
		return jobs::Job::CreateByLambda([=]() {
			updater->Tick(dt, jobs::Job::CreateByLambda(updaterDone));
		});
	};

	jobs::Job* getUpdaters = jobs::Job::CreateByLambda([=]() {
		ObjectValueContainer& container = ObjectValueContainer::GetContainer();

		container.GetObjectsOfType(runtime::TickUpdaterTypeDef::GetTypeDef(), updaters);

		bool anyUpdater = false;
		for (auto it = updaters.begin(); it != updaters.end(); ++it)
		{
			runtime::TickUpdater* cur = static_cast<runtime::TickUpdater*>(*it);
			if (!cur->IsTicking())
			{
				continue;
			}
			anyUpdater = true;

			++updatersRunning;
			jobs::RunAsync(startUpdater(cur));
		}

		if (!anyUpdater)
		{
			jobs::RunSync(done);
		}
	});

	jobs::RunSync(getUpdaters);
}

rendering::RendererAppEntryObj::RendererAppEntryObj(const ReferenceTypeDef& typeDef) :
	AppEntryObj(typeDef),
	m_renderer(renderer::RendererTypeDef::GetTypeDef(), this),
	m_copyBuffers(DXCopyBuffersTypeDef::GetTypeDef(), this),
	m_window(WindowTypeDef::GetTypeDef(), this)
{
}

rendering::RendererAppEntryObj::~RendererAppEntryObj()
{
	if (m_copyCommandLists)
	{
		delete[] m_copyCommandLists;
	}

	m_copyCommandLists = nullptr;
}

void rendering::RendererAppEntryObj::Boot()
{
	auto getRend = [=]() {
		return m_renderer.GetValue<renderer::RendererObj*>();
	};

	jobs::Job* load = jobs::Job::CreateByLambda([=]() {
		getRend()->Load(jobs::Job::CreateByLambda([=]() {
			Tick();
		}));
	});

	jobs::Job* init = jobs::Job::CreateByLambda([=]() {
		ObjectValueContainer::GetObjectOfType(renderer::RendererTypeDef::GetTypeDef(), m_renderer);
		ObjectValueContainer::GetObjectOfType(DXCopyBuffersTypeDef::GetTypeDef(), m_copyBuffers);
		ObjectValueContainer::GetObjectOfType(WindowTypeDef::GetTypeDef(), m_window);
		
		jobs::RunAsync(load);
 	});

	jobs::RunSync(init);
}

double rendering::RendererAppEntryObj::TimeStamp()
{
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	std::chrono::system_clock::time_point lastTickCache = m_lastTick;
	m_lastTick = now;

	if (!m_startedTicking)
	{
		return 0;
	}

	auto nowNN = std::chrono::time_point_cast<std::chrono::nanoseconds>(now);
	auto lastTickNN = std::chrono::time_point_cast<std::chrono::nanoseconds>(lastTickCache);
	long long deltaNN = nowNN.time_since_epoch().count() - lastTickNN.time_since_epoch().count();
	double dt = deltaNN / 1000000000.0;

	return dt;
}