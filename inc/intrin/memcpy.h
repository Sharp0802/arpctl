#ifndef ARPCTL_MEMCPY_H
#define ARPCTL_MEMCPY_H

#if __AVX2__

namespace intrin
{
#ifdef __clang__
	#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunknown-attributes"
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored_attributes "gnu::access"
#endif

	[[gnu::pure, gnu::access(write_only, 1, 3), gnu::access(read_only, 2, 3)]]
	void* memcpy(void* dst, const void* src, size_t n) noexcept;

#ifdef __clang__
#pragma clang diagnostic pop
#else
#pragma GCC diagnostic pop
#endif
}

#define _rt_memcpy(dst, src, n) intrin::memcpy(dst, src, n)

#else
#define _rt_memcpy std::memcpy
#endif

#endif //ARPCTL_MEMCPY_H
