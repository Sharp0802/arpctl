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

	volatile bool _token;
	volatile bool _run;

public:
	NetworkFlow(NetworkObject& sender, NetworkObject& target);

public:
	bool InitializeComponents();
	std::future<bool> InitializeComponentsDeferred();
	std::future<bool> InitializeComponentsAsync();

	std::future<bool> Run();
	std::future<bool> Relay();
	std::future<bool> Periodic(std::chrono::milliseconds period);

	std::future_status Join(std::chrono::milliseconds timeout);
	void Stop();
};


#endif //ARPCTL_NETFLOW_H
