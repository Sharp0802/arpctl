#ifndef ARPCTL_SPINLOCK_H
#define ARPCTL_SPINLOCK_H

#include "framework.h"


class Spinlock final
{
private:
	std::atomic<bool> _sync = { false };

public:
	void Lock();

	bool TryLock();

	void Unlock();
};

#endif //ARPCTL_SPINLOCK_H
