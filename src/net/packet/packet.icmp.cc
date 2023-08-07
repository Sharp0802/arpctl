#include "pch.h"
#include "net/packet/packet.icmp.h"

ICMPPacket::ICMPPacket(const ICMPPacket& rhs) :
		IPv4Packet(rhs._eth, rhs._ipv4),
		_icmp(rhs._icmp)
{
}

ICMPPacket::ICMPPacket(const EthernetHeader& eth, const IPv4Header& ipv4, const ICMP& icmp) :
		IPv4Packet(eth, ipv4),
		_icmp(icmp)
{
}

enum IPacket::Type ICMPPacket::GetType() const noexcept
{
	return IPacket::Type::ICMP;
}

std::vector<uint8_t> ICMPPacket::GetRaw() const noexcept
{
	std::vector<uint8_t> data(IPv4Packet::GetRaw());
	data.resize(GetSize());

	auto icmp = _icmp.Raw.Get();

	_rt_memcpy(data.data(), &icmp, sizeof(icmp));

	return data;
}

size_t ICMPPacket::GetSize() const noexcept
{
	return IPv4Packet::GetSize() + sizeof(DTO(ICMP));
}
