#ifndef ARPCTL_ARPHDR_H
#define ARPCTL_ARPHDR_H

#include "framework.h"
#include "convention.h"
#include "mac.h"
#include "ip.h"
#include "log.h"


decl_DTO(ARPHeader)
{
	uint16_t htype;
	uint16_t ptype;
	uint8_t hlen;
	uint8_t plen;
	uint16_t oper;
	DTO(MAC) sha;
	DTO(IP) spa;
	DTO(MAC) tha;
	DTO(IP) tpa;
};

class ARPHeader final
{
public:
	enum class HardwareType : std::uint16_t
	{
		Ethernet = 1
	};

	enum class ProtocolType : std::uint16_t
	{
		IPv4 = 0x0800
	};

	enum class OPCode : std::uint16_t
	{
		Request = 1,
		Reply = 2
	};

	static uint8_t GetHardwareTypeSize(HardwareType type);

	static uint8_t GetProtocolTypeSize(ProtocolType type);

private:
	DTO(ARPHeader) _dto;

public:
	ARPHeader();

	ARPHeader(const ARPHeader& rhs);

	explicit ARPHeader(void* raw);

public:
	property<DTO(ARPHeader)> Raw{
			_get
			{
				return _dto;
			},
			_set
			{
				_dto = value;
			}
	};

	property<HardwareType> Hardware{
			_get
			{
				return static_cast<enum HardwareType>(ntohs(_dto.htype));
			},
			_set
			{
				_dto.hlen = GetHardwareTypeSize(value);
				_dto.htype = htons(static_cast<uint16_t>(value));
			}
	};

	property<ProtocolType> Protocol{
			_get
			{
				return static_cast<enum ProtocolType>(ntohs(_dto.ptype));
			},
			_set
			{
				_dto.plen = GetProtocolTypeSize(value);
				_dto.ptype = htons(static_cast<uint16_t>(value));
			}
	};

	property<OPCode> OPCode{
			_get
			{
				return static_cast<enum OPCode>(ntohs(_dto.oper));
			},
			_set
			{
				_dto.oper = htons(static_cast<uint16_t>(value));
			}
	};

	property<MAC> SenderMAC{
			_get
			{
				return MAC(&_dto.sha);
			},
			_set
			{
				_dto.sha = value.Raw.Get();
			}
	};

	property<IP> SenderIP{
			_get
			{
				return IP(&_dto.spa);
			},
			_set
			{
				_dto.spa = value.Raw.Get();
			}
	};

	property<MAC> TargetMAC{
			_get
			{
				return MAC(&_dto.tha);
			},
			_set
			{
				_dto.tha = value.Raw.Get();
			}
	};

	property<IP> TargetIP{
			_get
			{
				return IP(&_dto.tpa);
			},
			_set
			{
				_dto.tpa = value.Raw.Get();
			}
	};
};


#endif //ARPCTL_ARPHDR_H
