#include "pch.h"
#include "pre/arphdr.h"

uint8_t ARPHeader::GetHardwareTypeSize(ARPHeader::HardwareType type)
{
	if (type == HardwareType::Ethernet)
		return 6;
	LOG(WARN) << "unknown hardware type("
			  << static_cast<uint16_t>(type)
			  << ") detected. default value(6) will be used";
	return 6;
}

uint8_t ARPHeader::GetProtocolTypeSize(ARPHeader::ProtocolType type)
{
	if (type == ProtocolType::IPv4)
		return 4;
	LOG(WARN) << "unknown protocol type("
			  << static_cast<uint16_t>(type)
			  << ") detected. default value(4) will be used";
	return 4;
}

ARPHeader::ARPHeader() : _dto({})
{
}

ARPHeader::ARPHeader(const ARPHeader& rhs) : _dto(rhs._dto)
{
}

ARPHeader::ARPHeader(void* raw) : _dto(*static_cast<DTO(ARPHeader)*>(raw))
{
}
