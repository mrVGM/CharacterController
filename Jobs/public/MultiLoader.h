#pragma once

#include "Job.h"

#include <list>

namespace jobs
{
	class LoadingClass
	{
		friend class MultiLoader;
	protected:
		virtual void LoadData(Job* done) = 0;

	public:
		virtual ~LoadingClass();
	};

	class MultiLoader
	{
	private:
		LoadingClass& m_loadingClass;
		std::list<jobs::Job*> m_loadRequests;

		bool m_loadStarted = false;
		bool m_loaded = false;

	public:
		MultiLoader(LoadingClass& loadingClass);

		void Load(jobs::Job* done);
	};
}