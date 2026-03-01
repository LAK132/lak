#include "lak/memmanip.hpp"
#include "lak/functional.hpp"
#include "lak/math.hpp"
#include "lak/ptr_intrin.hpp"
#include "lak/span.hpp"
#include "lak/span_manip.hpp"
#include "lak/system/compiler.hpp"

#include <cstdlib>

size_t lak::align_ptr_offset(byte_t *ptr, size_t align)
{
	return lak::align_ptr_offset(reinterpret_cast<uintptr_t>(ptr), align);
}

size_t lak::align_ptr_offset(uintptr_t ptr, size_t align)
{
	if (align <= 1U) return 0U;
	return (align - size_t(ptr % align)) % align; // slack
}

byte_t *lak::align_ptr(byte_t *ptr, size_t align)
{
	return ptr + lak::align_ptr_offset(ptr, align);
}

uintptr_t lak::align_ptr(uintptr_t ptr, size_t align)
{
	return ptr + lak::align_ptr_offset(ptr, align);
}

void lak::memmove(byte_t *dst, const byte_t *src, size_t count)
{
	if (lak::less<>{}(dst, src))
		for (size_t i = 0; i < count; ++i) dst[i] = src[i];
	else if (lak::greater<>{}(dst, src))
		while (count-- > 0) dst[count] = src[count];
}

void lak::memmove(lak::span<byte_t> dst, lak::span<const byte_t> src)
{
	lak::memmove(
	  dst.data(), src.data(), dst.size() < src.size() ? dst.size() : src.size());
}

void *lak::aligned_alloc(size_t alignment, size_t size)
{
#ifdef LAK_COMPILER_MSVC
	return _aligned_malloc(size, alignment);
#else
	return std::aligned_alloc(alignment, size);
#endif
}

void lak::aligned_free(void *p)
{
#ifdef LAK_COMPILER_MSVC
	return _aligned_free(p);
#else
	std::free(p);
#endif
}
