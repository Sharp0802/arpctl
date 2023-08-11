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
	_ipv4.Length = GetSize();
}

enum IPacket::Type ICMPPacket::GetType() const noexcept
{
	return IPacket::Type::ICMP;
}

OctetStream ICMPPacket::GetRaw() const noexcept
{
	OctetStream data(IPv4Packet::GetRaw());
	auto pl = &data.As<DTO(IPv4Header)>(IPv4Packet::GetSize() - sizeof(DTO(IPv4Header)))->len;
	if (*pl == IPv4Header::Auto)
		*pl = htons(GetSize() - sizeof(DTO(EthernetHeader)));

	auto icmp = _icmp.Raw.Get();
	icmp.chk = _icmp.CalculateChecksumWith(_payload);
	OctetStream sicmp(&icmp, sizeof(icmp));

	data += sicmp;
	data += _payload;

	return data;
}

size_t ICMPPacket::GetSize() const noexcept
{
	return IPv4Packet::GetSize() + sizeof(DTO(ICMP)) + _payload.Size();
}
