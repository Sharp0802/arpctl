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

std::vector<uint8_t> ICMPPacket::GetRaw() const noexcept
{
	std::vector<uint8_t> data(IPv4Packet::GetRaw());
	auto* pl = &reinterpret_cast<DTO(IPv4Header)*>(&data[IPv4Packet::GetSize() - sizeof(DTO(IPv4Header))])->len;
	if (*pl == IPv4Header::Auto)
		*pl = htons(GetSize() - sizeof(DTO(EthernetHeader)));
	data.resize(GetSize());

	std::cout << _payload.data() << '\n';

	auto icmp = _icmp.Raw.Get();
	icmp.chk = _icmp.CalculateChecksumWith(_payload);

	auto base = data.data() + IPv4Packet::GetSize();
	_rt_memcpy(base, &icmp, sizeof(icmp));
	_rt_memcpy(base + sizeof(DTO(ICMP)), _payload.data(), _payload.size());

	return data;
}

size_t ICMPPacket::GetSize() const noexcept
{
	return IPv4Packet::GetSize() + sizeof(DTO(ICMP)) + _payload.size();
}
