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

size_t lak::page_size()
{
#if defined(LAK_OS_WINDOWS)
  const static DWORD page_size = []() {
    SYSTEM_INFO system_info;
    GetSystemInfo(&system_info);
    return system_info.dwPageSize;
  }();
#elif defined(LAK_OS_LINUX)
  const static long page_size = sysconf(_SC_PAGESIZE);
#else
#  error "OS not supported"
#endif
  return page_size;
}

lak::span<void> lak::page_reserve(size_t size, size_t *page_size_out)
{
  ASSERT_GREATER(size, 0);
  void *result           = nullptr;
  const size_t page_size = lak::page_size();
  if (page_size_out) *page_size_out = page_size;
  // round size to the nearest page size
  size = page_size * ((size % page_size > 1 ? 1 : 0) + (size / page_size));

#if defined(LAK_OS_WINDOWS)
  result = VirtualAlloc(nullptr, size, MEM_RESERVE, PAGE_READWRITE);
#elif defined(LAK_OS_LINUX)
  if (int fd = open("/dev/zero", O_RDWR); fd >= 0)
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

bool lak::page_decommit(lak::span<void> pages)
{
// https://bugzilla.mozilla.org/show_bug.cgi?id=670596
#if defined(LAK_OS_WINDOWS)
  return VirtualFree(pages.data(), pages.size(), MEM_DECOMMIT) != 0;
#elif defined(LAK_OS_LINUX)
  return mprotect(pages.data(), pages.size(), PROT_NONE) == 0 &&
         madvise(pages.data(), pages.size(), MADV_DONTNEED) == 0;
#elif defined(LAK_OS_MAC)
  return mprotect(pages.data(), pages.size(), PROT_NONE) == 0 &&
         madvise(pages.data(), pages.size(), MADV_FREE) == 0;
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
