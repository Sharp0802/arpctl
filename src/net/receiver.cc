#include "pch.h"
#include "net/receiver.h"
#include "net/network_error.h"
#include "log.h"
#include "pre/arphdr.h"
#include "pre/icmp.h"
#include "pre/iphdr.h"
#include "pre/tcp.h"

EthernetHeader& HeaderSet::GetEthernetHeader() const noexcept
{
	return *_eth;
}

IPv4Header& HeaderSet::GetIPv4Header() const noexcept
{
	return *_ipv4;
}

ARPHeader& HeaderSet::GetARP() const noexcept
{
	return *_arp;
}

ICMP& HeaderSet::GetICMP() const noexcept
{
	return *_icmp;
}

TCP& HeaderSet::GetTCP() const noexcept
{
	return *_tcp;
}

std::optional<HeaderSet> HeaderSet::ParseFrom(const OctetStream& data) noexcept
{
	size_t ofs = 0;

	auto ethDTO = data.As<DTO(EthernetHeader)>(ofs);
	if (!ethDTO) return std::nullopt;
	auto eth = std::make_shared<EthernetHeader>(ethDTO);
	ofs += sizeof(DTO(EthernetHeader));

	if (eth->Type.Get() == EthernetHeader::EtherType::IPv4)
	{
		auto ipDTO = data.As<DTO(IPv4Header)>(ofs);
		if (!ipDTO) return std::nullopt;
		auto ip = std::make_shared<IPv4Header>(ipDTO);
		ofs += sizeof(DTO(IPv4Header));

		switch (ip->Protocol.Get())
		{
		case IPv4Header::Protocol::ICMP:
		{
			auto icmpDTO = data.As<DTO(ICMP)>(ofs);
			if (!icmpDTO) return std::nullopt;
			auto icmp = std::make_shared<::ICMP>(icmpDTO);
			ofs += sizeof(DTO(ICMP));

			return HeaderSet(eth, ip, icmp);
		}
		case IPv4Header::Protocol::TCP:
		{
			auto tcpDTO = data.As<DTO(TCP)>(ofs);
			if (!tcpDTO) return std::nullopt;
			auto tcp = std::make_shared<::TCP>(*ip, tcpDTO);
			ofs += tcp->DataOffset.Get();

			return HeaderSet(eth, ip, tcp);
		}
		default:
		{
			return HeaderSet(eth, ip);
		}
		}
	}
	else if (eth->Type.Get() == EthernetHeader::EtherType::ARP)
	{
		auto arpDTO = data.As<DTO(ARPHeader)>(ofs);
		if (!arpDTO) return std::nullopt;
		auto arp = std::make_shared<ARPHeader>(arpDTO);
		ofs += sizeof(DTO(ARPHeader));

		return HeaderSet(eth, arp);
	}
	else
	{
		return HeaderSet(eth);
	}
}

HeaderSet::SpecialType HeaderSet::GetType() const noexcept
{
	return _type;
}

void Receiver::Run(volatile const bool* token) const
{
	std::vector<uint8_t> buffer;
	while (*token)
	{
		struct pcap_pkthdr* hdr;
		const u_char* data;
		auto r = pcap_next_ex(_pcap.get(), &hdr, &data);
		switch (r)
		{
		case 0:
			break;
		case PCAP_ERROR:
		case PCAP_ERROR_BREAK:
		{
			constexpr const char msg[] = "could not retrieve next packet data.";
			LOG(CRIT) << msg;
			throw network_error(msg);
		}
		default:
			if (hdr->caplen < sizeof(DTO(EthernetHeader)))
				break;
			OctetStream stream(data, hdr->caplen);
			auto set = HeaderSet::ParseFrom(stream);
			if (!set)
			{
				LOG(WARN) << "invalid packet data detected. receiver will drop it.\n"
							 "=== PACKET DATA OPEN ===\n"
						  << static_cast<std::string>(stream)
						  << "\n=== PACKET DATA CLOSE ===";
				break;
			}

			_received(set.value(), stream);
			break;
		}
	}
}
