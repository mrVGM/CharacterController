#include "GCWorker.h"

#include "GC.h"

#include "Job.h"
#include "Jobs.h"

namespace
{
	bool m_shouldContinue = true;

	bool m_gcHasWorkToDo = true;
	bool m_gcIdle = false;

	struct Context
	{
		std::list<const gc::ManagedObject*> m_toDelete;
	};

	class DeleteManagedObjects : public jobs::Job
	{
		Context m_ctx;
	public:
		DeleteManagedObjects(const Context& ctx) :
			m_ctx(ctx)
		{
		}
		void Do() override;
	};

	class GCTick : public jobs::Job
	{
	public:
		void Do() override
		{
			Context ctx;
			gc::GCTick(ctx.m_toDelete);

			jobs::RunSync(new DeleteManagedObjects(ctx));
		}
	};

	void DeleteManagedObjects::Do()
	{
		for (auto it = m_ctx.m_toDelete.begin(); it != m_ctx.m_toDelete.end(); ++it)
		{
			const gc::ManagedObject* tmp = *it;
			delete tmp;
		}

		if (!m_shouldContinue)
		{
			return;
		}

		if (m_gcHasWorkToDo)
		{
			jobs::RunAsync(new GCTick());
			m_gcHasWorkToDo = false;
			return;
		}

		m_gcIdle = true;
	}

	class GCActivatedListener : public gc::GCActivatedListener
	{
	public:
		void OnActivated() const override
		{
			class ActivateGC : public jobs::Job
			{
			public:
				void Do() override
				{
					if (!m_shouldContinue)
					{
						return;
					}

					if (!m_gcIdle)
					{
						m_gcHasWorkToDo = true;
						return;
					}

					jobs::RunAsync(new GCTick());
				}
			};

			jobs::RunSync(new ActivateGC());
		}
	};

	GCActivatedListener m_listener;

	void BootGC()
	{
		gc::SetGCActivatedListener(m_listener);
		jobs::RunAsync(new GCTick());
	}
}

void gc::Boot()
{
	BootGC();
}

void gc::Shutdown()
{
	m_shouldContinue = false;
}
