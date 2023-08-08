#include "pch.h"
#include "pre/tcp.h"

TCP::TCP(const TCP& rhs) : _dto(rhs._dto)
{
}

TCP::TCP(const IPv4Header& iphdr, const void* raw) :
		_ip(iphdr),
		_dto(*static_cast<const DTO(TCP)*>(raw))
{
	UpdateChecksum();
}

/*
 * - RFC 1071 : Computing the Internet Checksum
 * - RFC 793  : Transmission Control Protocol
 *
 * The checksum field is the 16 bit one's complement of the one's
 * complement sum of all 16 bit words in the header and text.  If a
 * segment contains an odd number of header and text octets to be
 * checksummed, the last octet is padded on the right with zeros to
 * form a 16 bit word for checksum purposes.  The pad is not
 * transmitted as part of the segment.  While computing the
 * checksum, the checksum field itself is replaced with zeros.
 *
 * The checksum also covers a 96 bit pseudo header conceptually
 * prefixed to the TCP header.  This pseudo header contains the
 * Source Address, the Destination Address, the Protocol, and TCP
 * length. This gives the TCP protection against misrouted segments.
 * This information is carried in the Internet Protocol and is
 * transferred across the TCP/Network interface in the arguments or
 * results of calls by the TCP on the IP.
 *
 * The TCP Length is the TCP header length plus the data length in
 * octets (this is not an explicitly transmitted quantity, but is
 * computed), and it does not count the 12 octets of the pseudo
 * header.
 *
 * */
uint16_t TCP::CalculateChecksum() const noexcept
{
	thread_local struct {
		DTO(TCP) tcp;
		DTO(PseudoIPv4Header) pip;
	} data;

	_rt_memcpy(&data.tcp, &_dto, sizeof(data.tcp));
	data.tcp.chk = 0;

	auto ip = _ip.Raw.Get();

	data.pip.sip = ip.sip;
	data.pip.dip = ip.dip;
	data.pip.prtcl = ip.prtcl;
	data.pip.len = ip.len - sizeof(DTO(IPv4Header));

	return intrin::rfc1071(&data, sizeof(data));
}

void TCP::UpdateChecksum() noexcept
{
	_dto.chk = CalculateChecksum();
}
