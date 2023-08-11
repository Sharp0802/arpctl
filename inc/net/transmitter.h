#ifndef ARPCTL_TRANSMITTER_H
#define ARPCTL_TRANSMITTER_H

#include <utility>

#include "framework.h"
#include "pre/eth.h"
#include "pre/iphdr.h"
#include "pstl/concurrent_queue.h"
#include "dat/octet_stream.h"
#include "pstl/channeling_queue.h"

class Fragment final
{
private:
	const size_t _mtu;
	const std::vector<uint8_t> _data;

public:
	explicit Fragment(size_t mtu, std::vector<uint8_t> data) : _mtu(mtu), _data(std::move(data))
	{
	}

public:
	static std::optional<size_t> MTU(std::string_view interface);

public:
	[[nodiscard]]
	size_t Count() const;

	[[nodiscard]]
	size_t Size() const;

	[[nodiscard]]
	std::vector<uint8_t> Resolve() const;
};

class Transmitter final
{
private:
	std::shared_ptr<pcap_t> _pcap;
	pstl::channeling_queue<OctetStream> _q;

private:
	static Transmitter* _instance;

public:
	explicit Transmitter(std::shared_ptr<pcap_t> pcap);

	static Transmitter& GetInstance();

public:
	void Run(const volatile bool* token);

	void Transmit(OctetStream data);
	void Transmit(const void* data, size_t size);
};


#endif //ARPCTL_TRANSMITTER_H
