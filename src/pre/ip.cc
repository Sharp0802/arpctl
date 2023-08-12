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

std::optional<IP> IP::From(const std::string_view& str)
{
	thread_local std::array<char, 15> buffer;

	if (str.size() > 15)
		return std::nullopt;
	_rt_memcpy(buffer.data(), str.data(), 15);

	DTO(IP) dto{};
	for (size_t r = 0, i = 0, seg = 0; i < 15; ++i)
	{
		if (buffer[i] != '.')
			continue;
		buffer[i] = 0;
		if ((dto.seg[seg++] = std::strtol(&buffer[r], nullptr, 10)) > std::numeric_limits<uint8_t>::max())
			return std::nullopt;
		if (seg > 3)
			return std::nullopt;
	}

	return IP(&dto);
}

std::optional<IP> IP::Self(const std::string_view& interface)
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
		LOG(VERB) << "local IP detected: " << addr->ifa_name << ' ' << static_cast<std::string_view>(IP(raw));
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

IP::operator std::string_view() const
{
	std::string reserved;
	reserved.reserve(15);
	std::stringstream buf{ reserved };
	buf << _dto.seg[0] << '.' << _dto.seg[1] << '.' << _dto.seg[2] << '.' << _dto.seg[3];
	return { buf.str() };
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
