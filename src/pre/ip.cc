#include "pch.h"
#include "pre/ip.h"
#include "log.h"
#include "service/bootstrapper.h"
#include <ifaddrs.h>

IP::IP() : _dto({ 0, 0, 0, 0 })
{
}

IP::IP(const IP& rhs)
{
	Raw = rhs.Raw.Get();
}

IP::IP(void* raw)
{
	_rt_memcpy(&_dto, raw, sizeof(DTO(IP)));
}

IP::IP(DTO(IP) dto) : _dto(dto)
{
}

std::optional<IP> IP::From(const std::string& str)
{
	DTO(IP) dto{};
	std::sscanf(str.data(), "%hhd.%hhd.%hhd.%hhd", &dto.seg[0], &dto.seg[1], &dto.seg[2], &dto.seg[3]);

	return IP(&dto);
}

std::optional<IP> IP::Self(const std::string& interface)
{
	ifaddrs* addr;
	auto r = getifaddrs(&addr);
	if (r) return std::nullopt;
	ifaddrs* taddr = addr;

	bool init = false;
	DTO(IP) raw{};
	do
	{

		_rt_memcpy(raw.seg, &addr->ifa_addr->sa_data[2], 4);
		LOG(VERB) << "local IP detected: " << addr->ifa_name << ' ' << static_cast<std::string>(IP(raw));
		auto mask = static_cast<u_char>(addr->ifa_addr->sa_data[2]);
		if (interface == addr->ifa_name && (mask <= 223))
		{
			init = true;
			if (mask == 192 || mask == 172) // common masks
				break;
		}
	} while ((addr = addr->ifa_next));

	freeifaddrs(taddr);

	return init ? std::make_optional(IP(raw)) : std::nullopt;
}

std::optional<IP> IP::Self()
{
	auto interface = Bootstrapper::GetInstance().Device.Get();
	return Self(interface);
}

IP::operator std::string() const
{
	std::array<char, 32> buf{};
	std::sprintf(buf.data(), "%u.%u.%u.%u", _dto.seg[0], _dto.seg[1], _dto.seg[2], _dto.seg[3]);
	return { buf.data() };
}

IP& IP::operator=(const IP& rhs)
{
	Raw = rhs.Raw.Get();
	return *this;
}

bool IP::operator==(const IP& rhs) const
{
	for (size_t i = 0; i < 4; ++i)
		if (_dto.seg[i] != rhs._dto.seg[i])
			return false;
	return true;
}

bool IP::operator!=(const IP& rhs) const
{
	for (size_t i = 0; i < 4; ++i)
		if (_dto.seg[i] != rhs._dto.seg[i])
			return true;
	return false;
}

uint8_t IP::operator[](size_t i) const
{
	return Raw.Get().seg[i];
}
