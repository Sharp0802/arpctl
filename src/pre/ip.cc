#include "pch.h"
#include "pre/ip.h"

IP::IP() : _dto({ 0, 0, 0, 0 })
{
}

IP::IP(const IP& rhs)
{
	Raw = rhs.Raw.Get();
}

IP::IP(void* raw)
{
	std::memcpy(&_dto, raw, sizeof(DTO(IP)));
}

std::optional<IP> IP::From(const std::string_view& str)
{
	thread_local std::array<char, 15> buffer;

	if (str.size() > 15)
		return std::nullopt;
	std::memcpy(buffer.data(), str.data(), 15);

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

uint8_t IP::operator[](size_t i) const
{
	return Raw.Get().seg[i];
}
