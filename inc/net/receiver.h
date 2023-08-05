#ifndef ARPCTL_RECEIVER_H
#define ARPCTL_RECEIVER_H

#include "framework.h"
#include "event.h"
#include "pre/eth.h"


class Receiver final
{
private:
	inline static Receiver* _instance;

	std::shared_ptr<pcap_t> _pcap;
	Event<EthernetHeader, const std::vector<uint8_t>&> _received;

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

private:
	void Handle(size_t n, const uint8_t* data) const;

public:
	void Run(const volatile bool* token) const;
};


#endif //ARPCTL_RECEIVER_H
