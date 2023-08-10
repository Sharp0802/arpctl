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

std::vector<uint8_t> ARPPacket::GetRaw() const noexcept
{
	std::vector<uint8_t> raw(GetSize());
	auto arp = _arp.Raw.Get();
	auto eth = _eth.Raw.Get();
	_rt_memcpy(&raw[0], &eth, sizeof(DTO(EthernetHeader)));
	_rt_memcpy(&raw[sizeof(DTO(EthernetHeader))], &arp, sizeof(DTO(ARPHeader)));
	return raw;
}

size_t ARPPacket::GetSize() const noexcept
{
	return IPacket::GetSize() + sizeof(DTO(ARPHeader));
}
