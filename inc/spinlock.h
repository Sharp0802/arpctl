#ifndef ARPCTL_SPINLOCK_H
#define ARPCTL_SPINLOCK_H

#include "framework.h"


class spinlock final
{
private:
	std::atomic<bool> _sync = { false };

public:
	void lock();

	bool try_lock();

	void unlock();
};

#endif //ARPCTL_SPINLOCK_H
