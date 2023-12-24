#pragma once

#include <functional>

namespace jobs
{
	class Job
	{
	public:
		static Job* CreateByLambda(const std::function<void()>& lambda);

		virtual void Do() = 0;
		virtual ~Job();
	};
}