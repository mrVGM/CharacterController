#include "Job.h"

namespace
{
	class GenericJob : public jobs::Job
	{
	private:
		std::function<void()> m_lambda;

	public:
		GenericJob(const std::function<void()>& lambda) :
			m_lambda(lambda)
		{
		}

		void Do() override
		{
			m_lambda();
		}
	};
}

jobs::Job::~Job()
{
}

jobs::Job* jobs::Job::CreateByLambda(const std::function<void()>& lambda)
{
	return new GenericJob(lambda);
}