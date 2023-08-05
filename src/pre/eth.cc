#include "pch.h"
#include "pre/eth.h"

EthernetHeader::EthernetHeader() : _dto({ MAC::Unknown, MAC::Unknown, 0 })
{
}

EthernetHeader::EthernetHeader(MAC dmac, MAC smac, uint16_t type) : _dto({ dmac.Raw.Get(), smac.Raw.Get(), htons(type) })
{
}

EthernetHeader::EthernetHeader(void* rhs) : _dto(*static_cast<DTO(EthernetHeader)*>(rhs))
{
}
