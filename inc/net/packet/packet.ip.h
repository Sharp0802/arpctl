#ifndef ARPCTL_PACKET_IP_H
#define ARPCTL_PACKET_IP_H


#include "ipacket.h"
#include "pre/iphdr.h"

class IPv4Packet : protected IPacket
{
protected:
	IPv4Header _ipv4;

public:
	IPv4Packet() = delete;

	explicit IPv4Packet(const EthernetHeader& eth, const IPv4Header& ipv4);

public:
	[[nodiscard]]
	enum IPacket::Type GetType() const noexcept override;

	[[nodiscard]]
	OctetStream GetRaw() const noexcept override;

	[[nodiscard]]
	size_t GetSize() const noexcept override;
};


#endif //ARPCTL_PACKET_IP_H
