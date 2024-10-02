#pragma once

#include "Jobs.h"

#include <list>

namespace jobs
{
	class LoadingClass;

	class MultiLoader
	{
	private:
		LoadingClass& m_loadingClass;
		std::list<jobs::Job> m_loadRequests;

		bool m_loadStarted = false;
		bool m_loaded = false;

	public:
		MultiLoader(LoadingClass& loadingClass);

		void Load(jobs::Job done);
	};

	class LoadingClass
	{
		friend class MultiLoader;
	private:
		MultiLoader m_loader;
	protected:
		virtual void LoadData(Job done) = 0;

	public:
		LoadingClass();

		void Load(Job done);
		virtual ~LoadingClass();
	};
}