#include "lak/memmanip.hpp"
#include "lak/compiler.hpp"
#include "lak/span.hpp"

#if defined(LAK_OS_WINDOWS)
#  ifndef WIN32_MEAN_AND_LEAN
#    define WIN32_MEAN_AND_LEAN
#  endif
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  undef ERROR
#  include <Windows.h>
#elif defined(LAK_OS_LINUX)
#  include <fcntl.h>
#  include <sys/mman.h>
#  include <unistd.h>
#endif

lak::span<void> lak::page_reserve(size_t size)
{
  ASSERT_GREATER(size, 0);
  void *result = nullptr;
#if defined(LAK_OS_WINDOWS)
  const static DWORD page_size = []() {
    SYSTEM_INFO system_info;
    GetSystemInfo(&system_info);
    return system_info.dwPageSize;
  }();
  // round size to the nearest page size
  size   = page_size * ((size % page_size > 1 ? 1 : 0) + (size / page_size));
  result = VirtualAlloc(nullptr, size, MEM_RESERVE, PAGE_READWRITE);
#elif defined(LAK_OS_LINUX)
  const static long page_size = sysconf(_SC_PAGESIZE);
  // round size to the nearest page size
  size   = page_size * ((size % page_size > 1 ? 1 : 0) + (size / page_size));
  int fd = open("/dev/zero", O_RDWR);
  result = mmap(nullptr, size, PROT_NONE, MAP_PRIVATE | MAP_FILE, fd, 0);
#else
#  error "OS not supported"
#endif
  return lak::span<void>(result, result ? size : 0);
}

bool lak::page_commit(lak::span<void> pages)
{
#if defined(LAK_OS_WINDOWS)
  return VirtualAlloc(
           pages.data(), pages.size(), MEM_COMMIT, PAGE_READWRITE) != nullptr;
#elif defined(LAK_OS_LINUX)
  return mprotect(pages.data(), pages.size(), PROT_READ | PROT_WRITE) == 0;
#else
#  error "OS not supported"
#endif
}

bool lak::page_free(lak::span<void> pages)
{
#if defined(LAK_OS_WINDOWS)
  return VirtualFree(pages.data(), 0, MEM_RELEASE) != 0;
#elif defined(LAK_OS_LINUX)
  return munmap(pages.data(), pages.size()) == 0;
#else
#  error "OS not supported"
#endif
}
