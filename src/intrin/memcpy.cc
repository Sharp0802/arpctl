#include "framework.h"
#include "immintrin.h"
#include "intrin/memcpy.h"
#include "log.h"


#if __AVX2__

#define ymm(n) ymm##n
#define decl_ymm(n) register __m256i ymm(n) __asm__(_STR(ymm(n)))

#define ymm_load(src, ofs) ymm(ofs) = _mm256_loadu_si256(static_cast<const __m256i*>(src) + ofs)
#define ymm_store(dst, ofs) _mm256_storeu_si256(static_cast<__m256i*>(dst) + ofs, ymm(ofs))

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunknown-attributes"
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored_attributes "gnu::access"
#endif

[[gnu::always_inline, gnu::access(write_only, 1), gnu::access(read_only, 2)]]
inline static void mov(void* __restrict dst, const void* __restrict src, size_t n) noexcept
{
	if (n & 8)
	{
		*static_cast<uint64_t*>(dst) = *static_cast<const uint64_t*>(src);
		src = static_cast<const uint64_t*>(src) + 1;
		dst = static_cast<uint64_t*>(dst) + 1;
	}
	if (n & 4)
	{
		*static_cast<uint32_t*>(dst) = *static_cast<const uint32_t*>(src);
		src = static_cast<const uint32_t*>(src) + 1;
		dst = static_cast<uint32_t*>(dst) + 1;
	}
	if (n & 2)
	{
		*static_cast<uint16_t*>(dst) = *static_cast<const uint16_t*>(src);
		src = static_cast<const uint16_t*>(src) + 1;
		dst = static_cast<uint16_t*>(dst) + 1;
	}
	if (n & 1)
	{
		*static_cast<uint8_t*>(dst) = *static_cast<const uint8_t*>(src);
	}
}

[[gnu::always_inline, gnu::access(write_only, 1), gnu::access(read_only, 2)]]
inline static void mov16(void* __restrict dst, const void* __restrict src) noexcept
{
	__m128i xmm;
	xmm = _mm_loadu_si128(static_cast<const __m128i*>(src));
	_mm_storeu_si128(static_cast<__m128i*>(dst), xmm);
}

[[gnu::always_inline, gnu::access(write_only, 1), gnu::access(read_only, 2)]]
inline static void mov32(void* __restrict dst, const void* __restrict src) noexcept
{
	decl_ymm(0);
	ymm_load(src, 0);
	ymm_store(dst, 0);
}

[[gnu::always_inline, gnu::access(write_only, 1), gnu::access(read_only, 2)]]
inline static void mov64(void* __restrict dst, const void* __restrict src) noexcept
{
	decl_ymm(0);
	decl_ymm(1);
	ymm_load(src, 0);
	ymm_load(src, 1);
	ymm_store(dst, 0);
	ymm_store(dst, 1);
}

[[gnu::always_inline, gnu::access(write_only, 1), gnu::access(read_only, 2)]]
inline static void mov128(void* __restrict dst, const void* __restrict src) noexcept
{
	decl_ymm(0);
	decl_ymm(1);
	decl_ymm(2);
	decl_ymm(3);
	ymm_load(src, 0);
	ymm_load(src, 1);
	ymm_load(src, 2);
	ymm_load(src, 3);
	ymm_store(dst, 0);
	ymm_store(dst, 1);
	ymm_store(dst, 2);
	ymm_store(dst, 3);
}

[[gnu::always_inline, gnu::access(write_only, 1), gnu::access(read_only, 2)]]
inline static void mov256(void* __restrict dst, const void* __restrict src) noexcept
{
	decl_ymm(0);
	decl_ymm(1);
	decl_ymm(2);
	decl_ymm(3);
	decl_ymm(4);
	decl_ymm(5);
	decl_ymm(6);
	decl_ymm(7);
	ymm_load(src, 0);
	ymm_load(src, 1);
	ymm_load(src, 2);
	ymm_load(src, 3);
	ymm_load(src, 4);
	ymm_load(src, 5);
	ymm_load(src, 6);
	ymm_load(src, 7);
	ymm_store(dst, 0);
	ymm_store(dst, 1);
	ymm_store(dst, 2);
	ymm_store(dst, 3);
	ymm_store(dst, 4);
	ymm_store(dst, 5);
	ymm_store(dst, 6);
	ymm_store(dst, 7);
}

[[gnu::always_inline, gnu::access(write_only, 1, 3), gnu::access(read_only, 2, 3)]]
inline static void mov512blk(void* __restrict dst, const void* __restrict src, size_t n) noexcept
{
	decl_ymm(0);
	decl_ymm(1);
	decl_ymm(2);
	decl_ymm(3);
	decl_ymm(4);
	decl_ymm(5);
	decl_ymm(6);
	decl_ymm(7);
	decl_ymm(8);
	decl_ymm(9);
	decl_ymm(10);
	decl_ymm(11);
	decl_ymm(12);
	decl_ymm(13);
	decl_ymm(14);
	decl_ymm(15);
	while (n >= 128)
	{
		ymm_load(src, 0);
		ymm_load(src, 1);
		ymm_load(src, 2);
		ymm_load(src, 3);
		ymm_load(src, 4);
		ymm_load(src, 5);
		ymm_load(src, 6);
		ymm_load(src, 7);
		ymm_load(src, 8);
		ymm_load(src, 9);
		ymm_load(src, 10);
		ymm_load(src, 11);
		ymm_load(src, 12);
		ymm_load(src, 13);
		ymm_load(src, 14);
		ymm_load(src, 15);

		ymm_store(dst, 0);
		ymm_store(dst, 1);
		ymm_store(dst, 2);
		ymm_store(dst, 3);
		ymm_store(dst, 4);
		ymm_store(dst, 5);
		ymm_store(dst, 6);
		ymm_store(dst, 7);
		ymm_store(dst, 8);
		ymm_store(dst, 9);
		ymm_store(dst, 10);
		ymm_store(dst, 11);
		ymm_store(dst, 12);
		ymm_store(dst, 13);
		ymm_store(dst, 14);
		ymm_store(dst, 15);

		src = static_cast<const __m256i*>(src) + 16;
		dst = static_cast<__m256i*>(dst) + 16;
		n -= 128;
	}
}

[[gnu::access(write_only, 1, 3), gnu::access(read_only, 2, 3)]]
void* intrin::memcpy(void* __restrict dst, const void* __restrict src, size_t n) noexcept
{
	mov512blk(dst, src, n);
	n %= 512;
	if (n < 16)
	{
		mov(dst, src, n);
	}
	else if (n <= 32)
	{
		mov16(dst, src);
		mov16(static_cast<uint8_t*>(dst) - 16 + n, static_cast<const uint8_t*>(src) - 16 + n);
	}
	else if (n <= 48)
	{
		mov32(dst, src);
		mov16(static_cast<uint8_t*>(dst) - 16 + n, static_cast<const uint8_t*>(src) - 16 + n);
	}
	else if (n <= 64)
	{
		mov32(dst, src);
		mov32(static_cast<uint8_t*>(dst) - 32 + n, static_cast<const uint8_t*>(src) - 32 + n);
	}
	else if (n <= 128)
	{
		mov64(dst, src);
		mov64(static_cast<uint8_t*>(dst) - 64 + n, static_cast<const uint8_t*>(src) - 64 + n);
	}
	else if (n <= 256)
	{
		mov128(dst, src);
		mov128(static_cast<uint8_t*>(dst) - 128 + n, static_cast<const uint8_t*>(src) - 128 + n);
	}
	else if (n < 512)
	{
		mov256(dst, src);
		mov256(static_cast<uint8_t*>(dst) - 256 + n, static_cast<const uint8_t*>(src) - 256 + n);
	}

	return dst;
}

#ifdef __clang__
#pragma clang diagnostic pop
#else
#pragma GCC diagnostic pop
#endif

#endif
