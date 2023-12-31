#ifndef ARPCTL_PACKET_ARP_H
#define ARPCTL_PACKET_ARP_H

#include "ipacket.h"
#include "pre/arphdr.h"
#include "dat/octet_stream.h"


class ARPPacket final : public IPacket
{
private:
	const ARPHeader& _arp;

public:
	ARPPacket() = delete;

	ARPPacket(const ARPPacket& rhs);

	explicit ARPPacket(const EthernetHeader& eth, const ARPHeader& arp);

public:
	[[nodiscard]]
	enum IPacket::Type GetType() const noexcept override;

	[[nodiscard]]
	OctetStream GetRaw() const noexcept override;

	[[nodiscard]]
	size_t GetSize() const noexcept override;
};


#endif //ARPCTL_PACKET_ARP_H
