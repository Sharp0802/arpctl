#include "pch.h"
#include "pre/icmp.h"
#include "dat/octet_stream.h"

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
uint16_t ICMP::CalculateChecksum() const noexcept
{
	decltype(_raw) c = _raw;
	c.chk = 0;
	return intrin::rfc1071(&c, sizeof(_raw));
}

uint16_t ICMP::CalculateChecksumWith(const OctetStream& payload) const noexcept
{
	OctetStream stream(&_raw, sizeof(_raw));
	stream += payload;
	stream.As<DTO(ICMP)>(0)->chk = 0;
	return intrin::rfc1071(stream.Get(), stream.Size());
}

void ICMP::UpdateChecksum() noexcept
{
	_raw.chk = CalculateChecksum();
}
