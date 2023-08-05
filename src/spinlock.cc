#include "pch.h"
#include "spinlock.h"

void Spinlock::Lock()
{
	for (;;)
	{
		if (!_sync.exchange(true, std::memory_order_acquire))
			return;
		while (_sync.load(std::memory_order_relaxed))
			__builtin_ia32_pause();
	}
}

bool Spinlock::TryLock()
{
	return !_sync.load(std::memory_order_relaxed) &&
		   !_sync.exchange(true, std::memory_order_acquire);
}

void Spinlock::Unlock()
{
	_sync.store(false, std::memory_order_release);
}

void pstl::spinlock::lock()
{
	for (;;)
	{
		if (!_sync.exchange(true, std::memory_order_acquire))
			return;
		while (_sync.load(std::memory_order_relaxed))
			__builtin_ia32_pause();
	}
}

bool pstl::spinlock::try_lock()
{
	return !_sync.load(std::memory_order_relaxed) &&
		   !_sync.exchange(true, std::memory_order_acquire);
}

void pstl::spinlock::unlock()
{
	_sync.store(false, std::memory_order_release);
}
