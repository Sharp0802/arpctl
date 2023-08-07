#ifndef ARPCTL_IPHDR_H
#define ARPCTL_IPHDR_H

#include "framework.h"
#include "convention.h"
#include "ip.h"
#include "property.h"
#include "ichecksumable.h"


decl_DTO(IPv4Header)
{
	uint8_t ver: 4;
	uint8_t ihl: 4;
	uint8_t dscp: 6;
	uint8_t ecn: 2;
	uint16_t len;
	uint16_t id;
	uint16_t flag: 3;
	uint16_t fofs: 13;
	uint8_t ttl;
	uint8_t prtcl;
	uint16_t chk;
	DTO(IP) sip;
	DTO(IP) dip;
};

class IPv4Header final : public IChecksumable
{
private:
	DTO(IPv4Header) _dto;

public:
	IPv4Header();

	IPv4Header(const IPv4Header& rhs);

	explicit IPv4Header(void* raw);

public:
	uint16_t CalculateChecksum() noexcept override;

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
			return _dto.flag;
		},
		_set
		{
			_dto.flag = value;
			UpdateChecksum();
		}
	};

	property<uint16_t> FragOffset {
		_get
		{
			return ntohs(_dto.fofs);
		},
		_set
		{
			_dto.fofs = htons(value);
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

	property<uint8_t> Protocol {
		_get
		{
			return _dto.prtcl;
		},
		_set
		{
			_dto.prtcl = value;
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
