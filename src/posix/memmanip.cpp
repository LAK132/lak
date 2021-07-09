#include "lak/memmanip.hpp"
#include "lak/compiler.hpp"
#include "lak/os.hpp"
#include "lak/span.hpp"

#include "wrapper.hpp"

size_t lak::page_size()
{
  const static long page_size = sysconf(_SC_PAGESIZE);
  return page_size;
}

lak::page_result_t<lak::span<void>> lak::page_reserve(size_t size,
                                                      size_t *page_size_out)
{
  ASSERT_GREATER(size, 0);

  size = lak::round_to_page_multiple(size, page_size_out);

  auto ptr_to_span = [size](void *ptr) { return lak::span<void>(ptr, size); };

  return lak::posix::open("/dev/zero", O_RDWR)
    .and_then([size](int fd) {
      return lak::posix::mmap(
        nullptr, size, PROT_NONE, MAP_PRIVATE | MAP_FILE, fd, 0);
    })
    .map(ptr_to_span);
}

lak::page_result_t<> lak::page_commit(lak::span<void> pages)
{
  return lak::posix::mprotect(
    pages.begin(), pages.size(), PROT_READ | PROT_WRITE);
}

lak::page_result_t<> lak::page_free(lak::span<void> pages)
{
  return lak::posix::munmap(pages.data(), pages.size());
}