#ifndef ARPCTL_ICMP_H
#define ARPCTL_ICMP_H

#include "framework.h"
#include "convention.h"
#include "property.h"
#include "net/checksum.h"


decl_DTO(ICMP)
{
	uint8_t type;
	uint8_t code;
	uint16_t chk;
	uint32_t rst;
};

class ICMP final
{
public:
	enum class Type : std::uint8_t
	{
		EchoReply = 0,
		DestinationUnreachable = 3,
		SourceQuench = 4,
		RedirectMessage = 5,
		EchoRequest = 8,
		RouterAdvertisement = 9,
		RouterSolicitation = 10,
		TimeExceeded = 11,
		BadIPHeader = 12,
		Timestamp = 13,
		TimestampReply = 14,
		InformationRequest = 15,
		InformationReply = 16,
		AddressMaskRequest = 17,
		AddressMaskReply = 18,
		Traceroute = 30,
		ExtendedEchoRequest = 42,
		ExtendedEchoReply = 43
	};

	enum class Code : std::uint8_t
	{
		None = 0,

		// Destination Unreachable
		NetworkUnreachable = 0,
		HostUnreachable = 1,
		ProtocolUnreachable = 2,
		PortUnreachable = 3,
		FragmentRequired = 4,
		SourceRouteFailed = 5,
		NetworkUnknown = 6,
		HostUnknown = 7,
		SourceIsolated = 8,
		NetworkAdministrativelyProhibited = 9,
		HostAdministrativelyProhibited = 10,
		NetworkUnreachableForToS = 11,
		HostUnreachableForToS = 12,
		CommunicationAdministrativelyProhibited = 13,
		HostPrecedenceViolation = 14,
		PrecedenceCutoffInEffect = 15,

		// Redirect Message
		ForNetwork = 0,
		ForHost = 1,
		ForToSNetwork = 2,
		ForToSHost = 3,

		// Time Exceeded
		TTLExpired = 0,
		FragmentReassemblyTimeExceeded = 1,

		// Bad IP Header
		PointerIndicatesError = 0,
		MissingRequiredOption = 1,
		BadLength = 2,

		// Extended Echo Reply
		NoError = 0,
		MalformedQuery = 1,
		NoSuchInterface = 2,
		NoSuchTableEntry = 3,
		MultipleInterfacesSatisfyQuery = 4
	};

private:
	DTO(ICMP) _raw;

public:
	ICMP();

	ICMP(const ICMP& rhs);

	explicit ICMP(Type type, Code code, uint32_t data);

	explicit ICMP(const void* raw);

private:
	[[gnu::always_inline]]
	inline void UpdateChecksum() noexcept
	{
		_raw.chk = CalculateChecksum<ICMP>(&_raw);
	}

public:
	property<Type> Type {
		_get
		{
			return static_cast<enum Type>(_raw.type);
		},
		_set
		{
			_raw.type = static_cast<uint8_t>(value);
			UpdateChecksum();
		}
	};

	property<Code> Code {
		_get
		{
			return static_cast<enum Code>(_raw.code);
		},
		_set
		{
			_raw.code = static_cast<uint8_t>(value);
			UpdateChecksum();
		}
	};

	property<uint32_t> Value {
		_get
		{
			return ntohl(_raw.rst);
		},
		_set
		{
			_raw.rst = htonl(value);
			UpdateChecksum();
		}
	};

	readonly<uint16_t> Checksum {
		_get
		{
			return ntohl(_raw.chk);
		}
	};
};


#endif //ARPCTL_ICMP_H
