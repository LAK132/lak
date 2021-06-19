#include "lak/memmanip.hpp"
#include "lak/compiler.hpp"
#include "lak/os.hpp"
#include "lak/span.hpp"

#include "wrapper.hpp"

lak::page_result_t<> lak::page_decommit(lak::span<void> pages)
{
  // https://bugzilla.mozilla.org/show_bug.cgi?id=670596
  return lak::posix::mprotect(pages.data(), pages.size(), PROT_NONE)
    .and_then([&](...) {
      return lak::posix::madvise(pages.data(), pages.size(), MADV_FREE);
    })
    .map_err([](...) -> lak::page_error { return {}; });
}
