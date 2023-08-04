#ifndef ARPCTL_XTHREAD_H
#define ARPCTL_XTHREAD_H

#include "framework.h"

class worker
{
public:
	enum class state : std::uint8_t
	{
		COMPLETED = 0,
		RESERVED = 1,
		OCCUPIED = 2,
		EXCEPTION = 3
	};
};


#endif //ARPCTL_XTHREAD_H
