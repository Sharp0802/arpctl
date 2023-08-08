#ifndef ARPCTL_TCP_H
#define ARPCTL_TCP_H

#include "framework.h"
#include "convention.h"
#include "pre/iphdr.h"


decl_DTO(TCP)
{
	uint16_t sprt;
	uint16_t dprt;
	uint32_t seqn;
	uint32_t ackn;
	uint8_t ofs: 4;
	uint8_t rsvd: 4;
	uint8_t flag;
	uint16_t wndn;
	uint16_t chk;
	uint16_t urgp;
};

decl_DTO(PseudoIPv4Header)
{
	DTO(IP) sip;
	DTO(IP) dip;
	uint8_t fix;
	uint8_t prtcl;
	uint16_t len;
};

class TCP final : public IChecksumable
{
public:
	enum class Flag : uint8_t
	{
		URG = 0b100000,
		ACK = 0b010000,
		PSH = 0b001000,
		RST = 0b000100,
		SYN = 0b000010,
		FIN = 0b000001
	};

private:
	IPv4Header _ip;
	DTO(TCP) _dto;

public:
	// tcp header depends on ip header
	TCP() = delete;

	TCP(const TCP& rhs);

	explicit TCP(const IPv4Header& iphdr, const void* raw);

public:
	[[nodiscard]]
	uint16_t CalculateChecksum() const noexcept override;

private:
	void UpdateChecksum() noexcept;

public:
	property<DTO(TCP)> Raw{
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

	property<uint16_t> SourcePort{
			_get
			{
				return ntohs(_dto.sprt);
			},
			_set
			{
				_dto.sprt = htons(value);
				UpdateChecksum();
			}
	};

	property<uint16_t> DestinationPort{
			_get
			{
				return ntohs(_dto.dprt);
			},
			_set
			{
				_dto.dprt = htons(value);
				UpdateChecksum();
			}
	};

	property<uint32_t> SequenceNumber{
			_get
			{
				return ntohl(_dto.seqn);
			},
			_set
			{
				_dto.seqn = htonl(value);
				UpdateChecksum();
			}
	};

	property<uint32_t> AckNumber{
			_get
			{
				return ntohl(_dto.ackn);
			},
			_set
			{
				_dto.ackn = htonl(value);
				UpdateChecksum();
			}
	};

	property<uint8_t> DataOffset{
			_get
			{
				return _dto.ofs;
			},
			_set
			{
				_dto.ofs = value;
				UpdateChecksum();
			}
	};

	property<Flag> Flags{
		_get
		{
			return static_cast<Flag>(_dto.flag);
		},
		_set
		{
			_dto.flag = static_cast<uint8_t>(value);
			UpdateChecksum();
		}
	};

	property<uint16_t> Window{
		_get
		{
			return ntohs(_dto.wndn);
		},
		_set
		{
			_dto.wndn = htons(value);
			UpdateChecksum();
		}
	};

	readonly<uint16_t> Checksum{
		_get
		{
			return ntohs(_dto.chk);
		}
	};

	property<uint16_t> UrgentPointer{
		_get
		{
			return ntohs(_dto.urgp);
		},
		_set
		{
			_dto.urgp = htons(value);
			UpdateChecksum();
		}
	};
};


#endif //ARPCTL_TCP_H
