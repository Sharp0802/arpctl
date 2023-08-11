#include "pch.h"
#include "net/packet/packet.ip.h"

IPv4Packet::IPv4Packet(const EthernetHeader& eth, const IPv4Header& ipv4) : IPacket(eth), _ipv4(ipv4)
{
}

enum IPacket::Type IPv4Packet::GetType() const noexcept
{
	return IPacket::Type::IPv4;
}

OctetStream IPv4Packet::GetRaw() const noexcept
{
	auto eth = _eth.Raw.Get();
	auto ipv4 = _ipv4.Raw.Get();
	if (ipv4.len == IPv4Header::Auto)
		ipv4.len = htons(sizeof(ipv4));

	OctetStream seth(&eth, sizeof(eth));
	OctetStream sipv4(&ipv4, sizeof(ipv4));
	seth += sipv4;

	return seth;
}

size_t IPv4Packet::GetSize() const noexcept
{
	return IPacket::GetSize() + sizeof(DTO(IPv4Header));
}
