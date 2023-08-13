#ifndef ARPCTL_SNIFFER_H
#define ARPCTL_SNIFFER_H

#include "framework.h"
#include "net/netobj.h"


class Sniffer final
{
private:
	static std::unique_ptr<Sniffer> _instance;

public:
	Sniffer() = default;

public:
	static void Start();

private:
	void Handle(const HeaderSet& hdr, const OctetStream& data) noexcept;
};


#endif //ARPCTL_SNIFFER_H
