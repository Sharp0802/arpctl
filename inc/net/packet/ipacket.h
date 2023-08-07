#ifndef ARPCTL_IPACKET_H
#define ARPCTL_IPACKET_H

#include "framework.h"
#include "convention.h"
#include "pre/eth.h"


class IPacket
{
public:
	enum class Type
	{
		ARP,
		ICMP,
		IPv4,
		TCP
	};

protected:
	EthernetHeader _eth;

	explicit IPacket(const EthernetHeader& eth);

public:
	[[nodiscard]]
	virtual Type GetType() const noexcept = 0;

	[[nodiscard]]
	virtual std::vector<uint8_t> GetRaw() const noexcept = 0;

	[[nodiscard]]
	virtual size_t GetSize() const noexcept;
};


#endif //ARPCTL_IPACKET_H
