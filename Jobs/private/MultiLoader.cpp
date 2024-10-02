#include "MultiLoader.h"

#include "Jobs.h"

jobs::MultiLoader::MultiLoader(LoadingClass& loadingClass) :
	m_loadingClass(loadingClass)
{
}

void jobs::MultiLoader::Load(jobs::Job done)
{
	jobs::Job registerRequest = [=]() {

		if (m_loaded)
		{
			jobs::RunSync(done);
			return;
		}

		m_loadRequests.push_back(done);

		if (m_loadStarted)
		{
			return;
		}
		m_loadStarted = true;

		jobs::Job realLoad = [=]() {
			jobs::Job loadFinished = [=]() {
				m_loaded = true;
				for (auto it = m_loadRequests.begin(); it != m_loadRequests.end(); ++it)
				{
					jobs::RunSync(*it);
				}
				m_loadRequests.clear();
			};

			m_loadingClass.LoadData([=]() {
				jobs::RunSync(loadFinished);
			});
		};

		jobs::RunAsync(realLoad);
	};

	jobs::RunSync(registerRequest);
}

jobs::LoadingClass::LoadingClass() :
	m_loader(*this)
{
}

jobs::LoadingClass::~LoadingClass()
{
}

void jobs::LoadingClass::Load(jobs::Job done)
{
	m_loader.Load(done);
}