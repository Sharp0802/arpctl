#include "pch.h"
#include "pre/tcp.h"

TCP::TCP(const TCP& rhs) : _dto(rhs._dto)
{
}

TCP::TCP(const IPv4Header& iphdr, const void* raw) :
		_ip(iphdr),
		_dto(*static_cast<const DTO(TCP)*>(raw))
{
	UpdateChecksum();
}
