#ifndef ARPCTL_IPHDR_H
#define ARPCTL_IPHDR_H

#include "framework.h"
#include "convention.h"
#include "ip.h"
#include "property.h"


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

class IPv4Header final
{
private:
	DTO(IPv4Header) _dto;

public:
	IPv4Header();

	IPv4Header(const IPv4Header& rhs);

	explicit IPv4Header(void* raw);

public:
	property<uint8_t> Version {
		_get
		{
			return _dto.ver;
		},
		_set
		{
			_dto.ver = value;
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
		}
	};

	property<uint16_t> Checksum {
		_get
		{
			return _dto.chk;
		},
		_set
		{
			_dto.chk = value;
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
		}
	};
};


#endif //ARPCTL_IPHDR_H
