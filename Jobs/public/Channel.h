#pragma once

#include <threads.h>
#include <queue>

namespace jobs
{
	template <typename T>
	class Channel
	{
	private:
		bool m_initialized = false;

		mtx_t m_mtx;
		cnd_t m_cnd;

		std::queue<T> m_queue;

	public:
		Channel()
		{
			mtx_init(&m_mtx, mtx_plain);
			cnd_init(&m_cnd);
		}
		~Channel()
		{
			mtx_destroy(&m_mtx);
			cnd_destroy(&m_cnd);
		}

		void Push(const T& element)
		{
			mtx_lock(&m_mtx);
			m_queue.push(element);
			mtx_unlock(&m_mtx);
			cnd_broadcast(&m_cnd);
		}

		T Pop()
		{
			mtx_lock(&m_mtx);
			while (m_queue.empty())
			{
				cnd_wait(&m_cnd, &m_mtx);
			}
			
			T res = m_queue.front();
			m_queue.pop();

			mtx_unlock(&m_mtx);
			cnd_broadcast(&m_cnd);

			return res;
		}
	};
}
