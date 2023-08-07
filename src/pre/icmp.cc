#include "pch.h"
#include "pre/icmp.h"

ICMP::ICMP() : _raw({})
{
	UpdateChecksum();
}

ICMP::ICMP(const ICMP& rhs) : _raw(rhs._raw)
{
}

ICMP::ICMP(enum ICMP::Type type, enum ICMP::Code code, uint32_t data) :
		_raw(static_cast<uint8_t>(type), static_cast<uint8_t>(code), 0, htonl(data))
{
	UpdateChecksum();
}

ICMP::ICMP(const void* raw) : _raw(*static_cast<const DTO(ICMP)*>(raw))
{
}

/*
 * - RFC 1071 : Computing the Internet Checksum
 * - RFC 792  : Internet Control Message Protocol
 *
 * Internet checksum (RFC 1071) for error checking, calculated form
 * the ICMP header and data with value 0 substituted for this field
 *
 * */
uint16_t ICMP::CalculateChecksum() noexcept
{
	thread_local union
	{
		DTO(ICMP) dto;
		uint16_t u16[sizeof(dto) / 2];
	} buf;

	_rt_memcpy(&buf, &_raw, sizeof(buf));
	buf.dto.chk = 0;

	uint16_t r = 0;
	for (uint16_t i : buf.u16)
		intrin::_mm_add_1cmpl(&r, i);

	return r;
}

void ICMP::UpdateChecksum() noexcept
{
	_raw.chk = CalculateChecksum();
}
