#include "spinlock.h"

void spinlock::lock()
{
	for (;;)
	{
		if (!_sync.exchange(true, std::memory_order_acquire))
			return;
		while (_sync.load(std::memory_order_relaxed))
			__builtin_ia32_pause();
	}
}

bool spinlock::try_lock()
{
	return !_sync.load(std::memory_order_relaxed) &&
		   !_sync.exchange(true, std::memory_order_acquire);
}

void spinlock::unlock()
{
	_sync.store(false, std::memory_order_release);
}
