#ifndef ARPCTL_PACKET_ICMP_H
#define ARPCTL_PACKET_ICMP_H

#include "ipacket.h"
#include "pre/icmp.h"
#include "packet.ip.h"

class ICMPPacket final : protected IPv4Packet
{
private:
	ICMP _icmp;
	std::vector<uint8_t> _payload;

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

public:
	template<typename T>
	ICMPPacket& operator<<(T arg)
	{
		_payload.push_back(arg);
		return *this;
	}

	template<typename T, size_t N>
	ICMPPacket& operator<<(T argv[N])
	{
		_payload.reserve(_payload.size() + N);
		for (size_t i = 0; i < N; ++i)
			_payload.push_back(argv[i]);
		return *this;
	}

	ICMPPacket& operator<<(const char* str)
	{
		auto size = strlen(str);
		_payload.reserve(_payload.size() + size);
		for (size_t i = 0; i < size; ++i)
			_payload.push_back(str[i]);
		return *this;
	}
};


#endif //ARPCTL_PACKET_ICMP_H
