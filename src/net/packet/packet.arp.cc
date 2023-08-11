#include "net/packet/packet.arp.h"

ARPPacket::ARPPacket(const ARPPacket& rhs) : IPacket(rhs._eth), _arp(rhs._arp)
{
}

ARPPacket::ARPPacket(const EthernetHeader& eth, const ARPHeader& arp) : IPacket(eth), _arp(arp)
{
}

enum IPacket::Type ARPPacket::GetType() const noexcept
{
	return IPacket::Type::ARP;
}

OctetStream ARPPacket::GetRaw() const noexcept
{
	auto arp = _arp.Raw.Get();
	auto eth = _eth.Raw.Get();
	OctetStream sarp(&arp, sizeof(arp));
	OctetStream seth(&eth, sizeof(eth));
	sarp += seth;
	return sarp;
}

size_t ARPPacket::GetSize() const noexcept
{
	return IPacket::GetSize() + sizeof(DTO(ARPHeader));
}
