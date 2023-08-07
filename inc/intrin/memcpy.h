#ifndef ARPCTL_MEMCPY_H
#define ARPCTL_MEMCPY_H

#if __AVX2__

namespace intrin
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-attributes"

	[[gnu::pure, gnu::access(write_only, 1, 3), gnu::access(read_only, 2, 3)]]
	void* memcpy(void* dst, const void* src, size_t n) noexcept;

#pragma GCC diagnostic pop
}

#define _rt_memcpy intrin::memcpy

#else
#define _rt_memcpy std::memcpy
#endif

#endif //ARPCTL_MEMCPY_H
