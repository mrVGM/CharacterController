#include "GCWorker.h"

#include "GC.h"

#include "Job.h"
#include "Jobs.h"

namespace
{
	bool m_shouldContinue = true;

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
			delete* it;
		}

		if (m_shouldContinue)
		{
			jobs::RunAsync(new GCTick());
		}
	}

	void BootGC()
	{
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
