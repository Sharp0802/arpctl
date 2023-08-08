#include "intrin/rfc1071.h"

uint16_t intrin::rfc1071(const void* data, size_t n) noexcept
{
	size_t s = n + n % 2 / 1;
	auto* p = new uint8_t[s];
	p[s - 1] = 0;
	_rt_memcpy(p, data, n);

	const auto* u16 = reinterpret_cast<const uint16_t*>(p);
	uint16_t r = 0;
	for (size_t i = 0; i < s / 2; ++i)
		intrin::_mm_add_1cmpl(&r, *(u16++));

	return ~r;
}
