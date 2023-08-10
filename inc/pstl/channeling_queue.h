#ifndef ARPCTL_CHANNELING_QUEUE_H
#define ARPCTL_CHANNELING_QUEUE_H

#include "framework.h"
#include "spinlock.h"


namespace pstl
{
	template<typename T>
	class channeling_queue final
	{
	private:
		/*
		 * should NOT use generic readers-writer lock(std::shared_mutex)
		 * it's special scenario(ring-queue), there are more ways to optimize it
		 *
		 * */
		pstl::spinlock _rsync;
		pstl::spinlock _wsync;

		std::vector<T> _ring;
		volatile size_t _w;
		volatile size_t _r;
		volatile size_t _s;
		volatile size_t _c;

	public:
		channeling_queue()
		{
			_w = 0;
			_r = 0;
			_s = 0;
			_c = 16;

			_ring.resize(_c);
		}

	public:
		void push(T value) noexcept
		{
			std::lock_guard lock_w(_wsync);

			_ring[_w] = value;
			_w = (_w + 1) % _c;

			__sync_add_and_fetch(&_s, 1);

			/*
			 * do NOT remove duplicated branch statements
			 * it's intended design (double-checked locking)
			 *
			 * */
			if (_w == _r)
			{
				std::lock_guard lock_r(_rsync);

				if (_w == _r)
				{
					_ring.resize(_c * 2);
					_w = _c;
					_c *= 2;
				}
			}
		}

		[[nodiscard]]
		std::optional<T> pop() noexcept
		{
			T v;
			{
				std::lock_guard lock(_rsync);

				if (_r == _w)
					return std::nullopt;

				v = _ring[_r];
				_r = (_r + 1) % _c;

				__sync_sub_and_fetch(&_s, 1);
			}
			return v;
		}

		size_t size() const noexcept
		{
			return _s;
		}
	};
}


#endif //ARPCTL_CHANNELING_QUEUE_H
