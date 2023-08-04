#include "pre/ip.h"

IP::IP() : _dto({0, 0, 0, 0})
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

IP::IP(const std::string_view& str)
{
	std::sscanf(
			str.data(),
			"%hhu.%hhu.%hhu.%hhu",
			&_dto.seg[0],
			&_dto.seg[1],
			&_dto.seg[2],
			&_dto.seg[3]);
}

IP& IP::operator=(const IP& rhs)
{
	Raw = rhs.Raw.Get();
	return *this;
}

uint8_t IP::operator[](size_t i)
{
	return Raw.Get().seg[i];
}
