#ifndef ARPCTL_CHECKSUM_H
#define ARPCTL_CHECKSUM_H

#include "framework.h"
#include "pre/iphdr.h"
#include "pre/icmp.h"

#ifndef COMMENT
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
#endif

[[gnu::always_inline]]
inline void __add_wc(uint16_t* a, uint16_t b)
{
	uint16_t c = *a + b;
	*a = (c & 0xFF) + (c >> 16);
}


template<typename T>
[[gnu::pure]]
uint16_t CalculateChecksum(const void* data) noexcept;


/*
 * - RFC 1071 : Computing the Internet Checksum
 * - RFC 791  : Internet Header Format
 *
 * The checksum field is the 16-bit ones' complement of the ones'
 * complement sum of all 16-bit words in the "header". For purposes of
 * computing the checksum, the value of the checksum field is zero.
 *
 * */
template<>
[[gnu::pure]]
uint16_t CalculateChecksum<IPv4Header>(const void* data) noexcept
{
	thread_local union
	{
		DTO(IPv4Header) dto;
		uint16_t u16[sizeof(dto) / 2];
	} buf;

	_rt_memcpy(&buf, data, sizeof(buf.dto));
	buf.dto.chk = 0;

	uint16_t r = 0;
	for (uint16_t i : buf.u16)
		__add_wc(&r, i);
	return r;
}

/*
 * - RFC 1071 : Computing the Internet Checksum
 *
 * Internet checksum (RFC 1071) for error checking, calculated form
 * the ICMP header and data with value 0 substituted for this field
 *
 * */
template<>
[[gnu::pure]]
uint16_t CalculateChecksum<ICMP>(const void* data) noexcept
{
	thread_local union
	{
		DTO(ICMP) dto;
		uint16_t u16[sizeof(dto) / 2];
	} buf;

	_rt_memcpy(&buf, data, sizeof(buf));
	buf.dto.chk = 0;

	uint16_t r = 0;
	for (uint16_t i : buf.u16)
		__add_wc(&r, i);

	return r;
}


#endif //ARPCTL_CHECKSUM_H
