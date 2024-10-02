#pragma once


#include <functional>

namespace jobs
{
	typedef std::function<void()> Job;
	void Boot();

	void RunSync(const Job& job);
	void RunAsync(const Job& job);
}