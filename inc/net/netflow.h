#ifndef ARPCTL_NETFLOW_H
#define ARPCTL_NETFLOW_H

#include "framework.h"
#include "netobj.h"


class NetworkFlow final
{
private:
	NetworkObject& _sender;
	NetworkObject& _target;
	std::chrono::milliseconds _timeout;

public:
	NetworkFlow(
			NetworkObject& sender,
			NetworkObject& target,
			std::chrono::milliseconds timeout) :
			_sender(sender),
			_target(target),
			_timeout(timeout)
	{
	}

public:
	bool InitializeComponents();
	std::future<bool> InitializeComponentsDeferred();
	std::future<bool> InitializeComponentsAsync();

	std::future<bool> Run();
	std::future<bool> Relay();
	std::future<bool> Periodic(std::chrono::milliseconds period);
};


#endif //ARPCTL_NETFLOW_H
