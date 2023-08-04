#ifndef ARPCTL_XTHREAD_H
#define ARPCTL_XTHREAD_H

#include "framework.h"

class Worker
{
public:
	enum class State : std::uint8_t
	{
		COMPLETED = 0,
		RESERVED = 1,
		OCCUPIED = 2,
		EXCEPTION = 3
	};

public:
	static size_t ID();
};


#endif //ARPCTL_XTHREAD_H
