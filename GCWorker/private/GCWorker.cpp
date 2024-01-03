#include "GCWorker.h"

#include "GC.h"

#include "Job.h"
#include "Jobs.h"

#include <set>
#include <sstream>
#include <mutex>

namespace
{

	enum GCState
	{
		Idle,
		Running,
		HasWorkToDo,
		ShutDown
	};

	class GCHandler
	{
	private:
		std::mutex m_mutex;
		GCState m_state = Idle;

		void StartGCTick();

		void HandleState(GCState state)
		{
			if (m_state == GCState::ShutDown)
			{
				return;
			}

			if (state == GCState::ShutDown)
			{
				m_state = GCState::ShutDown;
				return;
			}

			if (state == GCState::Running)
			{
				m_state = GCState::Running;
				return;
			}

			if (state == GCState::HasWorkToDo)
			{
				if (m_state == GCState::Idle)
				{
					StartGCTick();
					return;
				}

				m_state = GCState::HasWorkToDo;
				return;
			}

			if (state == GCState::Idle)
			{
				if (m_state == GCState::HasWorkToDo)
				{
					StartGCTick();
					return;
				}
				m_state = GCState::Idle;
				return;
			}
		}

	public:
		void SetState(GCState state)
		{
			m_mutex.lock();
			HandleState(state);
			m_mutex.unlock();
		}
	};

	void GCHandler::StartGCTick()
	{
		m_state = GCState::Running;

		static std::list<const gc::ManagedObject*> toDelete;

		jobs::RunAsync(jobs::Job::CreateByLambda([&]() {
			toDelete.clear();
			gc::GCTick(toDelete);

			jobs::RunSync(jobs::Job::CreateByLambda([&]() {

				gc::GCLogger::m_log << "DEL ";
				for (auto it = toDelete.begin(); it != toDelete.end(); ++it)
				{
					gc::GCLogger::m_log << (*it)->GetId() << ' ';
					delete *it;
				}
				toDelete.clear();
				gc::GCLogger::m_log << std::endl;

				HandleState(GCState::Idle);
			}));
		}));
	}

	GCHandler m_gcHandler;

	class GCActivatedListener : public gc::GCActivatedListener
	{
	public:

		void OnActivated() const override
		{
			m_gcHandler.SetState(GCState::HasWorkToDo);
		}
	};

	GCActivatedListener m_listener;

	void BootGC()
	{
		gc::SetGCActivatedListener(m_listener);
		m_listener.OnActivated();
	}
}

void gc::Boot()
{
	BootGC();
}

void gc::Shutdown()
{
	m_gcHandler.SetState(GCState::ShutDown);
}
