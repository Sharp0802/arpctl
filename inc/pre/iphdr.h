#ifndef ARPCTL_IPHDR_H
#define ARPCTL_IPHDR_H

#include "framework.h"
#include "convention.h"
#include "ip.h"
#include "property.h"
#include "ichecksumable.h"


decl_DTO(IPv4Header)
{
	/*
	 * I don't know why I should swap ihl and ver.
	 * If you know, Please contact me.
	 */
	uint8_t ihl: 4;
	uint8_t ver: 4;
	uint8_t dscp: 6;
	uint8_t ecn: 2;
	uint16_t len;

	uint16_t id;
	uint16_t ffofs;

	uint8_t ttl;
	uint8_t prtcl;
	uint16_t chk;

	DTO(IP) sip;
	DTO(IP) dip;
};

class IPv4Header final : public IChecksumable
{
public:
	enum class Protocol : std::uint8_t
	{
		ICMP = 1,

		// RFC 3692
		Test0 = 253,
		Test1 = 254
	};

	static constexpr uint16_t Auto = -1;

private:
	DTO(IPv4Header) _dto;

public:
	IPv4Header();

	IPv4Header(const IPv4Header& rhs);

	explicit IPv4Header(void* raw);

public:
	[[nodiscard]]
	uint16_t CalculateChecksum() const noexcept override;

private:
	void UpdateChecksum();

public:
	property<DTO(IPv4Header)> Raw {
		_get
		{
			return _dto;
		},
		_set
		{
			_dto = value;
			UpdateChecksum();
		}
	};

	property<uint8_t> Version {
		_get
		{
			return _dto.ver;
		},
		_set
		{
			_dto.ver = value;
			UpdateChecksum();
		}
	};

	property<uint8_t> IHL {
		_get
		{
			return _dto.ihl;
		},
		_set
		{
			_dto.ihl = value;
			UpdateChecksum();
		}
	};

	property<uint8_t> DSCP {
		_get
		{
			return _dto.dscp;
		},
		_set
		{
			_dto.dscp = value;
			UpdateChecksum();
		}
	};

	property<uint8_t> ECN {
		_get
		{
			return _dto.ecn;
		},
		_set
		{
			_dto.ecn = value;
			UpdateChecksum();
		}
	};

	property<uint16_t> Length {
		_get
		{
			return ntohs(_dto.len);
		},
		_set
		{
			_dto.len = htons(value);
			UpdateChecksum();
		}
	};

	property<uint16_t> ID {
		_get
		{
			return ntohs(_dto.id);
		},
		_set
		{
			_dto.id = htons(value);
			UpdateChecksum();
		}
	};

	property<uint8_t> Flags {
		_get
		{
			return ntohs(_dto.ffofs & htons(0b11100000'00000000)) >> 13;
		},
		_set
		{
			_dto.ffofs &= htons(0b00011111'11111111);
			_dto.ffofs |= htons(static_cast<uint16_t>(value) << 13);
			UpdateChecksum();
		}
	};

	property<uint16_t> FragOffset {
		_get
		{
			return ntohs(_dto.ffofs & htons(0b00011111'11111111));
		},
		_set
		{
			_dto.ffofs &= htons(0b11100000'00000000);
			_dto.ffofs |= htons(value & 0b00011111'11111111);
			UpdateChecksum();
		}
	};

	property<uint8_t> TTL {
		_get
		{
			return _dto.ttl;
		},
		_set
		{
			_dto.ttl = value;
			UpdateChecksum();
		}
	};

	property<Protocol> Protocol {
		_get
		{
			return static_cast<enum Protocol>(_dto.prtcl);
		},
		_set
		{
			_dto.prtcl = static_cast<uint8_t>(value);
			UpdateChecksum();
		}
	};

	readonly<uint16_t> Checksum {
		_get
		{
			return ntohs(_dto.chk);
		}
	};

	property<IP> Source {
		_get
		{
			return IP(&_dto.sip);
		},
		_set
		{
			_dto.sip = value.Raw.Get();
			UpdateChecksum();
		}
	};

	property<IP> Destination {
		_get
		{
			return IP(&_dto.dip);
		},
		_set
		{
			_dto.dip = value.Raw.Get();
			UpdateChecksum();
		}
	};
};


#endif //ARPCTL_IPHDR_H
