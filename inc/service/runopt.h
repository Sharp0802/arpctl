#ifndef ARPCTL_RUNOPT_H
#define ARPCTL_RUNOPT_H

#include "framework.h"
#include "property.h"
#include "convention.h"


decl_DTO(RunOption)
{
	bool view_pkt;
	bool chk_icmp;
	std::chrono::milliseconds timeout;
};

struct RunOption final
{
private:
	inline static RunOption* _global;

private:
	DTO(RunOption) _dto;

public:
	explicit RunOption(DTO(RunOption) dto);

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

	readonly<std::chrono::milliseconds> Timeout{
			_get
			{
				return _dto.timeout;
			}
	};
};


#endif //ARPCTL_RUNOPT_H
