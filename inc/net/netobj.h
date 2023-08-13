#ifndef ARPCTL_NETOBJ_H
#define ARPCTL_NETOBJ_H

#include <utility>

#include "framework.h"
#include "pre/ip.h"
#include "pre/mac.h"
#include "dat/octet_stream.h"
#include "event.h"
#include "receiver.h"
#include "sniffer.h"


class NetworkObject final
{
public:
	using EventType = Event<const HeaderSet&, const OctetStream&>;
	using HandlerType = Event<const HeaderSet&, const OctetStream&>::Handler;

private:
	::IP _ip;
	::MAC _mac;
	std::mutex _sync;

	Receiver::HandlerType _handler;
	EventType _received;

private:
	void OnReceived(const HeaderSet& hdr, const OctetStream& data);

	std::future<bool> ResolveMACAsync();

public:
	bool InitializeComponents();

	std::future<bool> InitializeComponentsDeferred();

	std::future<bool> InitializeComponentsAsync();


	bool Infect(const IP& ip);

	std::future<bool> InfectDeferred(const IP& ip);

	std::future<bool> InfectAsync(const IP& ip);


	bool InfectionTest(const IP& ip, std::chrono::milliseconds timeout);

	std::future<bool> InfectionTestDeferred(const IP& ip, std::chrono::milliseconds timeout);

	std::future<bool> InfectionTestAsync(const IP& ip, std::chrono::milliseconds timeout);


	/* SEMANTIC USE ONLY : null object can call this member function */
	void Send(OctetStream data);

public:
	explicit NetworkObject(const IP& ip);

	~NetworkObject();

public:
	readonly<::IP> IP{
			_get
			{
				return _ip;
			}
	};

	readonly<::MAC> MAC{
			_get
			{
				return _mac;
			}
	};

public:
	void operator+=(HandlerType handler)
	{
		_received += std::move(handler);
	}

	void operator-=(HandlerType handler)
	{
		_received -= std::move(handler);
	}
};


#endif //ARPCTL_NETOBJ_H
