#include "sniffer.h"
#include "log.h"

std::unique_ptr<Sniffer> Sniffer::_instance = nullptr;

void Sniffer::Start()
{
	_instance = std::make_unique<Sniffer>();

	Receiver::HandlerType handler([](const auto& hdr, const auto& data)
	{
		_instance->Handle(hdr, data);
	});

	Receiver::GetInstance() += handler;
}

void Sniffer::Handle(const HeaderSet& hdr, const OctetStream& data) noexcept
{
	constexpr const char* const type[] = {
			" ETH:",
			" ARP:",
			"IPv4:",
			"ICMP:",
			" TCP:"
	};

	std::stringstream buf;
	buf << type[static_cast<size_t>(hdr.GetType())];

	switch (hdr.GetType())
	{
	case HeaderSet::SpecialType::IPv4:
	case HeaderSet::SpecialType::ICMP:
	case HeaderSet::SpecialType::TCP:
	{
		auto ipv4 = hdr.GetIPv4Header();

		std::vector<char> fieldBuf(16, ' ');
		fieldBuf[15] = 0;

		auto sip = static_cast<std::string>(ipv4.Source.Get());
		_rt_memcpy(&fieldBuf[fieldBuf.size() - sip.size()], sip.data(), sip.size());
		buf << fieldBuf.data() << " → ";

		std::fill(fieldBuf.begin(), fieldBuf.end() - 1, ' ');
		auto dip = static_cast<std::string>(ipv4.Destination.Get());
		buf << dip << "  ";

		break;
	}
	default:
	{
		auto eth = hdr.GetEthernetHeader();

		std::vector<char> fieldBuf(18, ' ');
		fieldBuf[17] = 0;

		auto smac = static_cast<std::string>(eth.Source.Get());
		_rt_memcpy(&fieldBuf[fieldBuf.size() - smac.size()], smac.data(), smac.size());
		buf << fieldBuf.data() << " → ";

		std::fill(fieldBuf.begin(), fieldBuf.end() - 1, ' ');
		auto dmac = static_cast<std::string>(eth.Destination.Get());
		buf << dmac << "  ";

		break;
	}
	}

	switch (hdr.GetType())
	{
	case HeaderSet::SpecialType::ARP:
	{
		auto arp = hdr.GetARP();
		if (arp.OPCode.Get() == ARPHeader::OPCode::Request)
		{
			buf << "Who has " << static_cast<std::string>(arp.TargetIP.Get())
				<< "? Tell " << static_cast<std::string>(arp.SenderIP.Get())
				<< '(' << static_cast<std::string>(arp.SenderMAC.Get())
				<< ").";
		}
		else
		{
			buf << "Hello, " << static_cast<std::string>(arp.TargetIP.Get())
				<< '(' << static_cast<std::string>(arp.TargetMAC.Get())
				<< "), I'm " << static_cast<std::string>(arp.SenderIP.Get())
				<< "(" << static_cast<std::string>(arp.SenderMAC.Get())
				<< ").";
		}
		break;
	}
	case HeaderSet::SpecialType::ICMP:
	{
		auto icmp = hdr.GetICMP();

		uint32_t value = icmp.Value.Get();
		buf << static_cast<uint8_t>(icmp.Code.Get())
			<< " id=" << (value >> 16)
			<< ", seq=" << (value & 0xFFFF);

		break;
	}
	default:
	{
		buf << "Packet with size: " << data.Size();
	}
	}

	LOG(VERB) << buf.str();
}
