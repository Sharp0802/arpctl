#include "pre/eth.h"

ETH::ETH() : _dto({ MAC::Unknown, MAC::Unknown, 0 })
{
}

ETH::ETH(MAC dmac, MAC smac, uint16_t type) : _dto({ dmac.Raw.Get(), smac.Raw.Get(), htons(type) })
{
}

ETH::ETH(void* rhs) : _dto(*static_cast<DTO(ETH)*>(rhs))
{
}
