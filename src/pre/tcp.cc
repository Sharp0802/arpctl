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
uint16_t TCP::CalculateChecksum() noexcept
{
	thread_local union
	{
		DTO(TCP) dto;
		uint16_t u16[sizeof(dto) / 2];
	} tcp;

	_rt_memcpy(&tcp, &_dto, sizeof(tcp));
	tcp.dto.chk = 0;

	thread_local union
	{
		DTO(PseudoIPv4Header) dto;
		uint16_t u16[sizeof(dto) / 2];
	} pip;

	auto ip = _ip.Raw.Get();

	pip.dto.sip = ip.sip;
	pip.dto.dip = ip.dip;
	pip.dto.prtcl = ip.prtcl;
	pip.dto.len = ip.len - sizeof(DTO(IPv4Header));

	uint16_t r = 0;
	for (uint16_t i : tcp.u16)
		intrin::_mm_add_1cmpl(&r, i);
	for (uint16_t i : pip.u16)
		intrin::_mm_add_1cmpl(&r, i);

	return r;
}

void TCP::UpdateChecksum() noexcept
{
	_dto.chk = CalculateChecksum();
}
