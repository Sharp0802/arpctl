#include "pch.h"
#include "net/packet/packet.ip.h"

IPv4Packet::IPv4Packet(const EthernetHeader& eth, const IPv4Header& ipv4) : IPacket(eth), _ipv4(ipv4)
{
}

enum IPacket::Type IPv4Packet::GetType() const noexcept
{
	return IPacket::Type::IPv4;
}

std::vector<uint8_t> IPv4Packet::GetRaw() const noexcept
{
	std::vector<uint8_t> data;
	data.resize(GetSize());

	auto eth = _eth.Raw.Get();
	auto ipv4 = _ipv4.Raw.Get();
	if (ipv4.len == IPv4Header::Auto)
		ipv4.len = htons(sizeof(ipv4));

	_rt_memcpy(data.data(), &eth, sizeof(eth));
	_rt_memcpy(data.data() + sizeof(eth), &ipv4, sizeof(ipv4));

	return { data };
}

size_t IPv4Packet::GetSize() const noexcept
{
	return IPacket::GetSize() + sizeof(DTO(IPv4Header));
}
