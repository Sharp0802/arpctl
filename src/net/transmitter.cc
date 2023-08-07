#include "pch.h"
#include "net/transmitter.h"
#include "log.h"

#include <net/if.h>
#include <sys/ioctl.h>

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
		std::memcpy(_buffer.data() + _mtu * i, _data.data(), ofs);
	}

	// FRAG PAYLOAD
	const uint8_t* payload = &_data[ofs];
	for (size_t i = 0; i < Count(); ++i)
	{
		std::memcpy(_buffer.data() + _mtu * i + ofs, payload + (_mtu - ofs) * i, _mtu - ofs);
	}

	*/

	return { _buffer };
}

void Transmitter::Run(volatile const bool* token)
{
	while (*token)
	{
		auto opt = _q.try_pop();
		if (!opt) sched_yield();
		auto& data = opt.value();

		auto r = pcap_inject(_pcap.get(), data.data(), data.size());
		if (r == PCAP_ERROR)
			LOG(FAIL) << "could not inject packet";
		else
			LOG(VERB) << "packet sent (" << data.size() << " requested, " << r << " sent)";
	}
}

void Transmitter::Transmit(const std::vector<uint8_t>& data)
{
	_q.push(data);
}

void Transmitter::Transmit(const void* data, size_t size)
{
	std::vector<uint8_t> buf;
	buf.resize(size);
	_rt_memcpy(buf.data(), data, size);
	_q.push(buf);
}
