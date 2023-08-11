#ifndef ARPCTL_PACKET_ICMP_H
#define ARPCTL_PACKET_ICMP_H

#include "ipacket.h"
#include "pre/icmp.h"
#include "packet.ip.h"

class ICMPPacket final : protected IPv4Packet
{
private:
	ICMP _icmp;
	OctetStream _payload;

public:
	ICMPPacket() = delete;

	ICMPPacket(const ICMPPacket& rhs);

	explicit ICMPPacket(const EthernetHeader& eth, const IPv4Header& ipv4, const ICMP& icmp);

public:
	[[nodiscard]]
	enum IPacket::Type GetType() const noexcept override;

	[[nodiscard]]
	OctetStream GetRaw() const noexcept override;

	[[nodiscard]]
	size_t GetSize() const noexcept override;

public:
	template<typename T>
	ICMPPacket& operator<<(T arg)
	{
		_payload += OctetStream(&arg, sizeof(arg));
		return *this;
	}

	template<typename T, size_t N>
	ICMPPacket& operator<<(T argv[N])
	{
		_payload += OctetStream(argv, sizeof(T) * N);
		return *this;
	}

	template<typename T, size_t N>
	ICMPPacket& operator<<(const std::array<T, N>& argv)
	{
		_payload += OctetStream(argv.data(), argv.size());
		return *this;
	}

	ICMPPacket& operator<<(const char* str)
	{
		_payload += OctetStream(str, strlen(str));
		return *this;
	}
};


#endif //ARPCTL_PACKET_ICMP_H
