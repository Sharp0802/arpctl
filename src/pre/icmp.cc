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
