#ifndef ARPCTL_SNIFFER_H
#define ARPCTL_SNIFFER_H

#include "framework.h"
#include "net/netobj.h"


class Sniffer final
{
public:
	Sniffer(NetworkObject& object);

private:
	void Handle(const HeaderSet& hdr, const OctetStream& data) noexcept;
};


#endif //ARPCTL_SNIFFER_H
