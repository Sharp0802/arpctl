#ifndef ARPCTL_PACKET_ICMP_H
#define ARPCTL_PACKET_ICMP_H

#include "ipacket.h"
#include "pre/icmp.h"
#include "packet.ip.h"

class ICMPPacket final : protected IPv4Packet
{
public:
	enum class Type
	{
		ARP,
		ICMP,
		IPv4,
		TCP
	};

private:
	ICMP _icmp;

public:
	ICMPPacket() = delete;

	ICMPPacket(const ICMPPacket& rhs);

	explicit ICMPPacket(const EthernetHeader& eth, const IPv4Header& ipv4, const ICMP& icmp);

public:
	[[nodiscard]]
	enum IPacket::Type GetType() const noexcept override;

	[[nodiscard]]
	std::vector<uint8_t> GetRaw() const noexcept override;

	[[nodiscard]]
	size_t GetSize() const noexcept override;
};


#endif //ARPCTL_PACKET_ICMP_H
