#include "lak/memmanip.hpp"
#include "lak/compiler.hpp"
#include "lak/os.hpp"
#include "lak/span.hpp"

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

void lak::memcpy(char *dst, const char *src, size_t count)
{
  for (size_t i = 0; i < count; ++i) dst[i] = src[i];
}

void lak::memmove(char *dst, const char *src, size_t count)
{
  if (dst < src)
    for (size_t i = 0; i < count; ++i) dst[i] = src[i];
  else if (dst > src)
    while (count-- > 0) dst[count] = src[count];
}

void lak::memcpy(lak::span<char> dst, lak::span<const char> src)
{
  lak::memcpy(
    dst.data(), src.data(), dst.size() < src.size() ? dst.size() : src.size());
}

void lak::memmove(lak::span<char> dst, lak::span<const char> src)
{
  lak::memmove(
    dst.data(), src.data(), dst.size() < src.size() ? dst.size() : src.size());
}

size_t lak::round_to_page_multiple(size_t size, size_t *page_size_out)
{
  const size_t page_size = lak::page_size();
  if (page_size_out) *page_size_out = page_size;
  return page_size * ((size % page_size > 0 ? 1 : 0) + (size / page_size));
}

#ifndef LAK_DONT_AUTO_COMPILE_PLATFORM_SPECIFICS
#  if defined(LAK_OS_WINDOWS)
#    include "win32/memmanip.cpp"
#  endif

#  if defined(LAK_OS_LINUX)
#    include "linux/memmanip.cpp"
#    include "posix/memmanip.cpp"
#  endif

#  if defined(LAK_OS_APPLE)
#    include "macos/memmanip.cpp"
#    include "posix/memmanip.cpp"
#  endif
#endif
