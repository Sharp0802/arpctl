#ifndef ARPCTL_ETH_H
#define ARPCTL_ETH_H

#include "framework.h"
#include "convention.h"
#include "mac.h"


decl_DTO(EthernetHeader)
{
	DTO(MAC) dmac;
	DTO(MAC) smac;
	uint16_t type;
};

class EthernetHeader final
{
public:
	enum class EtherType : std::uint16_t
	{
		IPv4 = 0x0800,
		ARP = 0x0806,
		IPv6 = 0x86DD
	};

private:
	DTO(EthernetHeader) _dto;

public:
	EthernetHeader();

	EthernetHeader(const EthernetHeader& rhs) = default;

	explicit EthernetHeader(MAC dmac, MAC smac, uint16_t type);

	explicit EthernetHeader(void* rhs);

public:
	readonly<MAC> Destination{
			_get
			{
				return MAC(&_dto.dmac);
			}
	};

	readonly<MAC> Source{
			_get
			{
				return MAC(&_dto.smac);
			}
	};

	readonly<enum EtherType> Type{
			_get
			{
				return static_cast<EtherType>(ntohs(_dto.type));
			}
	};
};


#endif //ARPCTL_ETH_H