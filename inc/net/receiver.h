#ifndef ARPCTL_RECEIVER_H
#define ARPCTL_RECEIVER_H

#include "framework.h"
#include "event.h"
#include "pre/eth.h"
#include "dat/octet_stream.h"
#include "pre/iphdr.h"
#include "pre/arphdr.h"
#include "pre/icmp.h"
#include "pre/tcp.h"


struct HeaderSet final
{
public:
	enum class SpecialType : std::uint8_t
	{
		EthernetFrame,
		ARP,
		IPv4,
		ICMP,
		TCP
	};

private:
	std::shared_ptr<EthernetHeader> _eth;
	std::shared_ptr<IPv4Header> _ipv4;
	std::shared_ptr<ARPHeader> _arp;
	std::shared_ptr<ICMP> _icmp;
	std::shared_ptr<TCP> _tcp;

	SpecialType _type;

private:
	HeaderSet(
			decltype(_eth) eth,
			decltype(_ipv4) ipv4,
			decltype(_arp) arp,
			decltype(_icmp) icmp,
			decltype(_tcp) tcp,
			SpecialType type) noexcept:
			_eth(std::move(eth)),
			_ipv4(std::move(ipv4)),
			_arp(std::move(arp)),
			_icmp(std::move(icmp)),
			_tcp(std::move(tcp)),
			_type(type)
	{
	}

public:
	explicit HeaderSet(
			decltype(_eth) eth) noexcept:
			HeaderSet(
					std::move(eth),
					nullptr,
					nullptr,
					nullptr,
					nullptr,
					SpecialType::EthernetFrame)
	{
	}

	explicit HeaderSet(
			decltype(_eth) eth,
			decltype(_ipv4) ipv4) noexcept:
			HeaderSet(
					std::move(eth),
					std::move(ipv4),
					nullptr,
					nullptr,
					nullptr,
					SpecialType::IPv4)
	{
	}

	explicit HeaderSet(
			decltype(_eth) eth,
			decltype(_ipv4) ipv4,
			decltype(_icmp) icmp) noexcept:
			HeaderSet(
					std::move(eth),
					std::move(ipv4),
					nullptr,
					std::move(icmp),
					nullptr,
					SpecialType::ICMP)
	{
	}

	explicit HeaderSet(
			decltype(_eth) eth,
			decltype(_ipv4) ipv4,
			decltype(_tcp) tcp) noexcept:
			HeaderSet(
					std::move(eth),
					std::move(ipv4),
					nullptr,
					nullptr,
					std::move(tcp),
					SpecialType::TCP)
	{
	}

	explicit HeaderSet(
			decltype(_eth) eth,
			decltype(_arp) arp) noexcept:
			HeaderSet(
					std::move(eth),
					nullptr,
					std::move(arp),
					nullptr,
					nullptr,
					SpecialType::ARP)
	{
	}

public:
	[[nodiscard]] EthernetHeader& Ethernet() const noexcept;

	[[nodiscard]] IPv4Header& IPv4() const noexcept;

	[[nodiscard]] ARPHeader& ARP() const noexcept;

	[[nodiscard]] ICMP& ICMP() const noexcept;

	[[nodiscard]] TCP& TCP() const noexcept;

	[[nodiscard]] SpecialType Type() const noexcept;

public:
	[[nodiscard, gnu::pure]]
	static std::optional<HeaderSet> ParseFrom(const OctetStream& data) noexcept;
};

class Receiver final
{
private:
	inline static Receiver* _instance;

	std::shared_ptr<pcap_t> _pcap;
	Event<const HeaderSet&, const OctetStream&> _received;

public:
	explicit Receiver(std::shared_ptr<pcap_t> pcap) : _pcap(std::move(pcap))
	{
		_instance = this;
	}

	static Receiver& GetInstance()
	{
		return *_instance;
	}

public:
	void operator+=(decltype(_received)::Handler handler)
	{
		_received += std::move(handler);
	}

	void operator-=(decltype(_received)::Handler handler)
	{
		_received -= std::move(handler);
	}

public:
	void Run(const volatile bool* token) const;
};


#endif //ARPCTL_RECEIVER_H
