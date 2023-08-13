#ifndef ARPCTL_TRANSMITTER_TEST_APPLICATION_H
#define ARPCTL_TRANSMITTER_TEST_APPLICATION_H

#include "framework.h"
#include "iapplication.h"
#include "log.h"
#include "net/transmitter.h"
#include "net/packet/packet.icmp.h"
#include "net/netobj.h"
#include "bootstrapper.h"

using namespace std::chrono_literals;


class TransmitterTestApplication : public IApplication
{
private:
	std::string _dev;
	std::vector<std::tuple<std::string, std::string>> _pairs;
	MAC _selfMAC;
	IP _selfIP;

	std::shared_ptr<NetworkObject> _obj;

public:
	~TransmitterTestApplication() override = default;

	bool Configure(const std::vector<std::string>& argv) override
	{
		if (argv.size() % 2 != 1 || argv.size() < 3)
		{
			LOG(CRIT) << "invalid syntax";
			LOG(NOTE) << "syntax: arpctl <interface> [<sender-ip> <target-ip>]...\n"
						 "sample: arpctl wlan0 192.168.10.2 192.168.10.1";
			return false;
		}

		_dev = argv[0];
		for (size_t i = 1; i < argv.size(); i += 2)
			_pairs.emplace_back(argv[i], argv[i + 1]);

		auto mac = MAC::Self(_dev);
		if (!mac) return false;
		_selfMAC = mac.value();

		auto ip = IP::Self(_dev);
		if (!ip) return false;
		_selfIP = ip.value();

		return true;
	}

	void Start() override
	{
		std::array<char, PCAP_ERRBUF_SIZE> err{};
		std::shared_ptr<pcap_t> pcap(
				pcap_open_live(_dev.data(), BUFSIZ, 1, 1, err.data()),
				[](pcap_t* p) { pcap_close(p); }
		);
		if (pcap == nullptr)
		{
			LOG(FAIL) << "couldn't open device" << _dev << '(' << std::string(err.data()) << ")\n";
			return;
		}

		Bootstrapper bt(_dev, 1000ms);
		bt.Start();

		_obj = std::make_shared<NetworkObject>(IP::From(bt.Device.Get()).value());

		_obj->InitializeComponents();

		EthernetHeader eth(MAC::Broadcast, _selfMAC, EthernetHeader::EtherType::IPv4);

		IPv4Header ipv4;
		ipv4.Version = 4;
		ipv4.IHL = 5;
		ipv4.DSCP = 0;
		ipv4.ECN = 0;
		ipv4.Length = IPv4Header::Auto;
		ipv4.ID = rand();
		ipv4.Flags = 0x2; // Don't Fragment
		ipv4.FragOffset = 0;
		ipv4.TTL = 64;
		ipv4.Protocol = IPv4Header::Protocol::ICMP;
		ipv4.Source = _selfIP;
		ipv4.Destination = IP({ 8, 8, 8, 8 });

		ICMP icmp(ICMP::Type::EchoRequest, ICMP::Code::None, 0x0000'0001);

		ICMPPacket packet(eth, ipv4, icmp);
		packet << "0123456789:;<=>?";

		_obj->Send(packet.GetRaw());

		sleep(1000);
		bt.Abort();
	}

	enum Worker::State Join() override
	{
		return Worker::State::COMPLETED;
	}

	void Abort() override
	{

	}
};


#endif //ARPCTL_TRANSMITTER_TEST_APPLICATION_H
