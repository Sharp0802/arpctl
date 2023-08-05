#include "pch.h"
#include "net/receiver.h"
#include "net/network_error.h"
#include "log.h"

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
			if (hdr->caplen < sizeof(DTO(ETH)))
				break;
			Handle(hdr->caplen, data);
			break;
		}
	}
}

void Receiver::Handle(size_t size, const uint8_t* data) const
{
	ETH eth(const_cast<uint8_t*>(data));

	std::vector<uint8_t> buf;
	buf.resize(size - sizeof(ETH));
	std::memcpy(buf.data(), data + sizeof(ETH), buf.size());

	_received(eth, buf);
}
