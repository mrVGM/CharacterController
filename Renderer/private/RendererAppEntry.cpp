#include "RendererAppEntry.h"

#include "AppEntry.h"

#include "Jobs.h"
#include "ObjectValueContainer.h"
#include "Renderer.h"

#include "DXMutableBuffer.h"

#include "TickUpdater.h"

#include "CoreUtils.h"

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
	static int m_running;;
	m_running = 2;


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

	auto startUpdater = [=](TickUpdater* updater) {
		return jobs::Job::CreateByLambda([=]() {
			updater->Tick(dt, jobs::Job::CreateByLambda(updaterDone));
		});
	};

	jobs::Job* getUpdaters = jobs::Job::CreateByLambda([=]() {
		ObjectValueContainer& container = ObjectValueContainer::GetContainer();

		container.GetObjectsOfType(TickUpdaterTypeDef::GetTypeDef(), updaters);

		bool anyUpdater = false;
		for (auto it = updaters.begin(); it != updaters.end(); ++it)
		{
			TickUpdater* cur = static_cast<TickUpdater*>(*it);
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
	m_copyBuffers(DXCopyBuffersTypeDef::GetTypeDef(), this)
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
		ObjectValue* rend = ObjectValueContainer::GetObjectOfType(renderer::RendererTypeDef::GetTypeDef());
		m_renderer.AssignObject(rend);

		ObjectValue* copyBuffers = ObjectValueContainer::GetObjectOfType(DXCopyBuffersTypeDef::GetTypeDef());
		m_copyBuffers.AssignObject(copyBuffers);

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

#if false

void rendering::RendererAppEntryObj::Boot()
{
	struct Context
	{
		RendererAppEntryObj* m_self = nullptr;
	};

	Context ctx{ this };

	class RenderFrame : public jobs::Job
	{
	private:
		Context m_ctx;

	public:
		RenderFrame(const Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			renderer::RendererObj* renderer = m_ctx.m_self->m_renderer.GetValue<renderer::RendererObj*>();
			renderer->RenderFrame();
			jobs::RunAsync(new RenderFrame(m_ctx));
		}
	};

	class RendererLoaded : public jobs::Job
	{
	private:
		Context m_ctx;

	public:
		RendererLoaded(const Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			renderer::RendererObj* renderer = m_ctx.m_self->m_renderer.GetValue<renderer::RendererObj*>();
			jobs::RunAsync(new RenderFrame(m_ctx));
		}
	};

	class LoadRenderer : public jobs::Job
	{
	private:
		Context m_ctx;

	public:
		LoadRenderer(const Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			std::list<ObjectValue*> tmp;
			ObjectValueContainer& container = ObjectValueContainer::GetContainer();
			container.GetObjectsOfType(renderer::RendererTypeDef::GetTypeDef(), tmp);

			renderer::RendererObj* renderer = static_cast<renderer::RendererObj*>(tmp.front());
			m_ctx.m_self->m_renderer.AssignObject(renderer);

			renderer->Load(new RendererLoaded(m_ctx));
		}
	};

	jobs::RunSync(new LoadRenderer(ctx));

}

#endif