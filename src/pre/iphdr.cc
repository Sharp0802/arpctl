#include "pch.h"
#include "pre/iphdr.h"

IPv4Header::IPv4Header() : _dto({ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {0, }, {0, }})
{
}

IPv4Header::IPv4Header(const IPv4Header& rhs) : _dto(rhs._dto)
{
}

IPv4Header::IPv4Header(void* raw) : _dto(*static_cast<DTO(IPv4Header)*>(raw))
{
}
