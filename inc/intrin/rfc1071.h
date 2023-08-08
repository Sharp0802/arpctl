#ifndef ARPCTL_RFC1071_H
#define ARPCTL_RFC1071_H

#include "framework.h"


namespace intrin
{
	[[gnu::pure, gnu::access(read_only, 1, 2)]]
	uint16_t rfc1071(const void* data, size_t n) noexcept;
}

#endif //ARPCTL_RFC1071_H
