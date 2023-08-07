#ifndef ARPCTL_CONCURRENT_QUEUE_H
#define ARPCTL_CONCURRENT_QUEUE_H

#include "spinlock.h"

namespace pstl
{
	template<typename T>
	class concurrent_queue final
	{
	private:
		std::queue<T> _q;
		pstl::spinlock _sync;
		std::condition_variable _cond;

	public:
		void push(T item)
		{
			std::unique_lock lock(_sync);
			_q.push(item);
			_cond.notify_one();
		}

		T pop()
		{
			std::unique_lock lock(_sync);
			_cond.wait(lock, [this]() { return !_q.empty(); });

			auto item = _q.front();
			_q.pop();

			return item;
		}
	};
}

#endif //ARPCTL_CONCURRENT_QUEUE_H
