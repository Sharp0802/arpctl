#include "pre/mac.h"

[[nodiscard]]
static consteval std::array<char, 16> GetHexTable()
{
	std::array<char, 16> buf{};
	for (size_t i = 0; i < 10; ++i)
		buf[i] = i + '0';
	for (size_t i = 0; i < 6; ++i)
		buf[i + 10] = i + 'A';

	return buf;
}

static void DumpHex(char* dst, uint8_t src)
{
	std::array<char, 16> vt = GetHexTable();
	dst[0] = vt[src & 0xF0 >> 4];
	dst[1] = vt[src & 0x0F >> 0];
}

MAC::MAC() : _dto({ 0, 0, 0, 0, 0, 0 })
{
}

MAC::MAC(const MAC& rhs)
{
	_dto = rhs._dto;
}

MAC::MAC(void* raw)
{
	std::memcpy(&_dto, raw, sizeof(DTO(MAC)));
}

MAC::operator std::string_view() const
{
	std::array<char, 17> buffer{};
	for (size_t i = 0; i < 6; ++i)
		DumpHex(&buffer[3 * i], _dto.seg[i]);
	for (size_t i = 0; i < 5; ++i)
		buffer[i * 3 + 2] = ':';
	return { std::string(buffer.data()) }; // Using raw buffer causes returning temporary variable
}

MAC& MAC::operator=(const MAC& rhs)
{
	Raw = rhs.Raw.Get();
}

uint8_t MAC::operator[](size_t i) const
{
	return _dto.seg[i];
}
