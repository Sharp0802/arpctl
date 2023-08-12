#include "pch.h"
#include "net/transmitter.h"
#include "log.h"

#include <net/if.h>
#include <sys/ioctl.h>

Transmitter* Transmitter::_instance = nullptr;

std::optional<size_t> Fragment::MTU(std::string_view interface)
{
	int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
	if (fd == -1)
		return std::nullopt;

	struct ifreq ifr{};
	strcpy(ifr.ifr_name, interface.data());
	if (ioctl(fd, SIOCGIFMTU, &ifr))
		return std::nullopt;

	size_t mtu = ifr.ifr_mtu;

	close(fd);

	return mtu;
}

size_t Fragment::Count() const
{
	return (_data.size() / _mtu) + (_data.size() % _mtu) / 1;
}

size_t Fragment::Size() const
{
	size_t size = _data.size() - sizeof(DTO(EthernetHeader)) - sizeof(DTO(IPv4Header));
	return Count() * (sizeof(DTO(EthernetHeader)) + sizeof(DTO(IPv4Header))) + size;
}

std::vector<uint8_t> Fragment::Resolve() const
{
	static thread_local std::vector<uint8_t> _buffer;
	_buffer.resize(Size());

	/*

	constexpr size_t ofs = sizeof(DTO(EthernetHeader)) + sizeof(DTO(IPv4Header));

	// COPY HEADER
	for (size_t i = 0; i < Count(); ++i)
	{
		_rt_memcpy(_buffer.data() + _mtu * i, _payload.data(), ofs);
	}

	// FRAG PAYLOAD
	const uint8_t* payload = &_payload[ofs];
	for (size_t i = 0; i < Count(); ++i)
	{
		_rt_memcpy(_buffer.data() + _mtu * i + ofs, payload + (_mtu - ofs) * i, _mtu - ofs);
	}

	*/

	return { _buffer };
}

void Transmitter::Run(volatile const bool* token)
{
	while (*token)
	{
		auto opt = _q.pop();
		if (!opt)
		{
			sched_yield();
			continue;
		}
		auto& data = opt.value();

		auto r = pcap_inject(_pcap.get(), data.Get(), data.Size());
		if (r == PCAP_ERROR)
			LOG(FAIL) << "could not inject packet (" << pcap_geterr(_pcap.get()) << ')';
		else
			LOG(VERB) << "packet sent (" << data.Size() << " requested, " << r << " sent)";
	}
}

void Transmitter::Transmit(OctetStream data)
{
	_q.push(std::move(data));
}

void Transmitter::Transmit(const void* data, size_t size)
{
	_q.push(OctetStream(data, size));
}

Transmitter::Transmitter(std::shared_ptr<pcap_t> pcap) : _pcap(std::move(pcap))
{
	_instance = this;
}

Transmitter& Transmitter::GetInstance()
{
	return *_instance;
}
