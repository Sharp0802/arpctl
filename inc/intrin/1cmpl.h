#ifndef ARPCTL_1CMPL_H
#define ARPCTL_1CMPL_H

#include "framework.h"

/*
 * - RFC 1071 : Computing the Internet Checksum
 *
 * + (A) Introduction
 * 1. Adjacent octets to be checksummed are paired to form 16-bit
 *    integers, and the 1's complement sum of these 16-bit integers is
 *    formed.
 *
 * 2. To generate a checksum, the checksum field itself is cleared,
 *    the 16-bit 1's complement sum is computed over the octets
 *    concerned, and the 1's complement of this sum is placed in the
 *    checksum field.
 *
 * 3. To check a checksum, the 1's complement sum is computed over the
 *    same set of octets, including the checksum field.  If the result
 *    is all 1 bits (-0 in 1's complement arithmetic), the check
 *    succeeds.
 *
 * + (B) Byte Order Independence
 * > The sum of 16-bit integers can be computed in either byte order
 *
 * */

namespace intrin
{
	[[gnu::always_inline]]
	inline void _mm_add_1cmpl(uint16_t* r, uint16_t b) noexcept
	{
		uint32_t c = *r + b;
		*r = (c & 0xFFFF) + (c >> 16);
	}
}

#endif //ARPCTL_1CMPL_H
