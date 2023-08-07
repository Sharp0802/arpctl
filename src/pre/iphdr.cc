#include "pch.h"
#include "pre/iphdr.h"

IPv4Header::IPv4Header() : _dto({})
{
	UpdateChecksum();
}

IPv4Header::IPv4Header(const IPv4Header& rhs) : _dto(rhs._dto)
{
}

IPv4Header::IPv4Header(void* raw) : _dto(*static_cast<DTO(IPv4Header)*>(raw))
{
}

/*
 * - RFC 1071 : Computing the Internet Checksum
 * - RFC 791  : Internet Header Format
 *
 * The checksum field is the 16-bit ones' complement of the ones'
 * complement sum of all 16-bit words in the "header". For purposes of
 * computing the checksum, the value of the checksum field is zero.
 *
 * */
uint16_t IPv4Header::CalculateChecksum() noexcept
{
	thread_local union
	{
		DTO(IPv4Header) dto;
		uint16_t u16[sizeof(dto) / 2];
	} buf;

	_rt_memcpy(&buf, &_dto, sizeof(buf.dto));
	buf.dto.chk = 0;

	uint16_t r = 0;
	for (uint16_t i : buf.u16)
		intrin::_mm_add_1cmpl(&r, i);
	return r;
}

void IPv4Header::UpdateChecksum()
{
	_dto.chk = CalculateChecksum();
}
