#include "lak/memmanip.hpp"
#include "lak/compiler.hpp"
#include "lak/functional.hpp"
#include "lak/math.hpp"
#include "lak/os.hpp"
#include "lak/ptr_intrin.hpp"
#include "lak/span.hpp"
#include "lak/span_manip.hpp"

#include <cstdlib>

byte_t *lak::align_ptr(byte_t *ptr, size_t align)
{
	return reinterpret_cast<byte_t *>(
	  lak::align_ptr(reinterpret_cast<uintptr_t>(ptr), align));
}

uintptr_t lak::align_ptr(uintptr_t ptr, size_t align)
{
	if (align <= 1) return ptr;
	uintptr_t offset = ptr % align;
	return ptr + (offset == 0 ? 0 : align - offset);
}

void lak::memcpy(byte_t *dst, const byte_t *src, size_t count)
{
	for (size_t i = 0; i < count; ++i) dst[i] = src[i];
}

void lak::memmove(byte_t *dst, const byte_t *src, size_t count)
{
	if (lak::less<>{}(dst, src))
		for (size_t i = 0; i < count; ++i) dst[i] = src[i];
	else if (lak::greater<>{}(dst, src))
		while (count-- > 0) dst[count] = src[count];
}

void lak::memcpy(lak::span<byte_t> dst, lak::span<const byte_t> src)
{
	lak::memcpy(
	  dst.data(), src.data(), dst.size() < src.size() ? dst.size() : src.size());
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

size_t lak::round_to_page_multiple(size_t size, size_t *page_size_out)
{
	const size_t page_size = lak::page_size();
	if (page_size_out) *page_size_out = page_size;
	return size + lak::slack<size_t>(size, page_size);
}

#ifndef LAK_DONT_AUTO_COMPILE_PLATFORM_SPECIFICS
#	if defined(LAK_OS_WINDOWS)
#		include "win32/memmanip.cpp"
#		include "win32/wrapper.cpp"
#	endif

#	if defined(LAK_OS_LINUX) || defined(LAK_OS_APPLE)
#		include "posix/memmanip.cpp"
#	endif

#endif
