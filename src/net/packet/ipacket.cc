#include "pch.h"
#include "net/packet/ipacket.h"

IPacket::IPacket(const EthernetHeader& eth) : _eth(eth)
{
}

size_t IPacket::GetSize() const noexcept
{
	return sizeof(DTO(EthernetHeader));
}
