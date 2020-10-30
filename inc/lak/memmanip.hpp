#ifndef LAK_MEMMANIP_HPP
#define LAK_MEMMANIP_HPP

#include "lak/compiler.hpp"
#include "lak/span.hpp"

#if defined(LAK_OS_WINDOWS)
#elif defined(LAK_OS_LINUX)
#  include <fcntl.h>
#  include <sys/mman.h>
#  include <unistd.h>
#endif

#include <cstring>

namespace lak
{
  template<typename T, typename U>
  force_inline void memcpy(T *dst, const U *src)
  {
    static_assert(std::is_same_v<T, U>, "Cannot memcpy different types");
    std::memcpy(dst, src, sizeof(T));
  }

  template<typename T, typename U>
  force_inline void memmove(T *dst, const U *src)
  {
    static_assert(std::is_same_v<T, U>, "Cannot memmove different types");
    std::memmove(dst, src, sizeof(T));
  }

  template<typename T>
  force_inline void memset(T *dst, int val)
  {
    std::memset(dst, val, sizeof(T));
  }

  lak::span<void> page_reserve(size_t size)
  {
    ASSERT_GREATER(size, 0);
    void *result = nullptr;
#if defined(LAK_OS_WINDOWS)
#elif defined(LAK_OS_LINUX)
    const static long page_size = sysconf(_SC_PAGESIZE);
    // round size to the nearest page size
    size   = page_size * ((size % page_size > 1 ? 1 : 0) + (size / page_size));
    int fd = open("/dev/zero", O_RDWR);
    result = mmap(nullptr, size, PROT_NONE, MAP_PRIVATE | MAP_FILE, fd, 0);
#else
#endif
    return lak::span<void>(result, result ? size : 0);
  }

  bool page_commit(lak::span<void> pages)
  {
#if defined(LAK_OS_WINDOWS)
#elif defined(LAK_OS_LINUX)
    return mprotect(pages.data(), pages.size(), PROT_READ | PROT_WRITE) == 0;
#else
    return false;
#endif
  }

  bool page_free(lak::span<void> pages)
  {
#if defined(LAK_OS_WINDOWS)
#elif defined(LAK_OS_LINUX)
    return munmap(pages.data(), pages.size()) == 0;
#else
    return false;
#endif
  }
}

#endif