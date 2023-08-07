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
