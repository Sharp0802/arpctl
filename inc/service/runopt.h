#ifndef ARPCTL_RUNOPT_H
#define ARPCTL_RUNOPT_H

#include "framework.h"
#include "property.h"
#include "convention.h"


decl_DTO(RunOption)
{
	bool view_pkt;
	bool chk_icmp;
	bool use_sniffer;
	std::chrono::milliseconds timeout;
};

struct RunOption final
{
private:
	inline static RunOption* _global;

private:
	DTO(RunOption) _dto;

public:
	RunOption(DTO(RunOption) dto);

	static const RunOption& Global();

public:
	readonly<bool> ViewPacket{
			_get
			{
				return _dto.view_pkt;
			}
	};

	readonly<bool> CheckICMP{
			_get
			{
				return _dto.chk_icmp;
			}
	};

	readonly<bool> UseSniffer{
			_get
			{
				return _dto.use_sniffer;
			}
	};

	readonly<std::chrono::milliseconds> Timeout{
			_get
			{
				return _dto.timeout;
			}
	};
};


#endif //ARPCTL_RUNOPT_H
