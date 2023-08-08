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
uint16_t ICMP::CalculateChecksum() const noexcept
{
	decltype(_raw) c = _raw;
	c.chk = 0;
	return intrin::rfc1071(&c, sizeof(_raw));
}

uint16_t ICMP::CalculateChecksumWith(std::vector<uint8_t> payload) const noexcept
{
	std::vector<uint8_t> p;
	p.resize(sizeof(_raw) + payload.size());

	_rt_memcpy(&p[0], &_raw, sizeof(_raw));
	_rt_memcpy(&p[sizeof(_raw)], payload.data(), payload.size());
	reinterpret_cast<decltype(_raw)*>(&p[0])->chk = 0;

	return intrin::rfc1071(p.data(), p.size());
}

void ICMP::UpdateChecksum() noexcept
{
	_raw.chk = CalculateChecksum();
}
