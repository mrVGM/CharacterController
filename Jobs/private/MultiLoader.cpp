#include "MultiLoader.h"

#include "Jobs.h"

jobs::MultiLoader::MultiLoader(LoadingClass& loadingClass) :
	m_loadingClass(loadingClass)
{
}

void jobs::MultiLoader::Load(jobs::Job* done)
{
	jobs::Job* registerRequest = jobs::Job::CreateByLambda([=]() {

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

		jobs::Job* realLoad = jobs::Job::CreateByLambda([=]() {
			jobs::Job* loadFinished = jobs::Job::CreateByLambda([=]() {
				m_loaded = true;
				for (auto it = m_loadRequests.begin(); it != m_loadRequests.end(); ++it)
				{
					jobs::RunSync(*it);
				}
				m_loadRequests.clear();
			});

			m_loadingClass.LoadData(jobs::Job::CreateByLambda([=]() {
				jobs::RunSync(loadFinished);
			}));
		});

		jobs::RunAsync(realLoad);
	});

	jobs::RunSync(registerRequest);
}

jobs::LoadingClass::~LoadingClass()
{
}
