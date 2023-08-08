#include "pch.h"
#include "pre/eth.h"

EthernetHeader::EthernetHeader() : _dto({ MAC::UnknownDTO, MAC::UnknownDTO, 0 })
{
}

EthernetHeader::EthernetHeader(const MAC& dmac, const MAC& smac, EtherType type) :
		_dto({ dmac.Raw.Get(), smac.Raw.Get(), htons(static_cast<uint16_t>(type)) })
{
}

EthernetHeader::EthernetHeader(void* rhs) : _dto(*static_cast<DTO(EthernetHeader)*>(rhs))
{
}
