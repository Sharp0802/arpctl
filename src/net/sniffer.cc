#include "sniffer.h"
#include "log.h"

Sniffer::Sniffer(NetworkObject& object)
{
	Receiver::HandlerType handler([this](const auto& hdr, const auto& data)
	{
		Handle(hdr, data);
	});

	object += handler;
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

		auto sip = static_cast<std::string_view>(ipv4.Source.Get());
		_rt_memcpy(&fieldBuf[fieldBuf.size() - sip.size()], sip.data(), sip.size());
		buf << fieldBuf.data() << " → ";

		std::fill(fieldBuf.begin(), fieldBuf.end() - 1, ' ');
		auto dip = static_cast<std::string_view>(ipv4.Destination.Get());
		buf << dip << "  ";

		break;
	}
	default:
	{
		auto eth = hdr.GetEthernetHeader();

		std::vector<char> fieldBuf(18, ' ');
		fieldBuf[17] = 0;

		auto smac = static_cast<std::string_view>(eth.Source.Get());
		_rt_memcpy(&fieldBuf[fieldBuf.size() - smac.size()], smac.data(), smac.size());
		buf << fieldBuf.data() << " → ";

		std::fill(fieldBuf.begin(), fieldBuf.end() - 1, ' ');
		auto dmac = static_cast<std::string_view>(eth.Destination.Get());
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
			buf << "Who has " << static_cast<std::string_view>(arp.TargetIP.Get())
				<< "? Tell " << static_cast<std::string_view>(arp.SenderIP.Get())
				<< '(' << static_cast<std::string_view>(arp.SenderMAC.Get())
				<< ").";
		}
		else
		{
			buf << "Hello, " << static_cast<std::string_view>(arp.TargetIP.Get())
				<< '(' << static_cast<std::string_view>(arp.TargetMAC.Get())
				<< "), I'm " << static_cast<std::string_view>(arp.SenderIP.Get())
				<< "(" << static_cast<std::string_view>(arp.SenderMAC.Get())
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
