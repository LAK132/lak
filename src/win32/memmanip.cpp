#include "lak/memmanip.hpp"
#include "lak/compiler.hpp"
#include "lak/os.hpp"
#include "lak/span.hpp"

#include "wrapper.hpp"

size_t lak::page_size()
{
	const static DWORD page_size = []()
	{
		SYSTEM_INFO system_info;
		::GetSystemInfo(&system_info);
		return system_info.dwPageSize;
	}();

	return page_size;
}

lak::page_result_t<lak::span<void>> lak::page_reserve(size_t size,
                                                      size_t *page_size_out)
{
	ASSERT_GREATER(size, 0);

	size = lak::round_to_page_multiple(size, page_size_out);

	auto ptr_to_span = [size](void *ptr) { return lak::span<void>(ptr, size); };

	return lak::winapi::virtual_alloc(nullptr, size, MEM_RESERVE, PAGE_READWRITE)
	  .map(ptr_to_span)
	  .map_err([](auto &&) -> lak::page_error { return {}; });
}

lak::page_result_t<> lak::page_commit(lak::span<void> pages)
{
	return lak::winapi::virtual_alloc(
	         pages.data(), pages.size(), MEM_COMMIT, PAGE_READWRITE)
	  .map([](auto &&) -> lak::monostate { return {}; })
	  .map_err([](auto &&) -> lak::page_error { return {}; });
}

lak::page_result_t<> lak::page_decommit(lak::span<void> pages)
{
	return lak::winapi::virtual_free(pages.data(), pages.size(), MEM_DECOMMIT)
	  .map_err([](auto &&) -> lak::page_error { return {}; });
}

lak::page_result_t<> lak::page_free(lak::span<void> pages)
{
	return lak::winapi::virtual_free(pages.data(), 0, MEM_RELEASE)
	  .map_err([](auto &&) -> lak::page_error { return {}; });
}

#if 0
lak::page_result_t<bool> lak::pages_are_committed(lak::span<void> pages)
{
  for (;;)
  {
    if (pages.empty()) return lak::ok_t{false};
    const auto query = lak::winapi::virtual_query(pages.data());
    if (query.is_err()) return lak::err_t{query.unwrap_err()};
    const auto &info = query.unwrap();
    if (info.State != MEM_COMMIT) return lak::ok_t{false};
    if (info.RegionSize >= pages.size()) return lak::ok_t{true};
    pages =
      lak::span<void>(lak::span<uint8_t>(pages).subspan(info.RegionSize));
  }
}

lak::page_result_t<bool> lak::pages_are_reserved(lak::span<void> pages)
{
  for (;;)
  {
    if (pages.empty()) return lak::ok_t{false};
    const auto query = lak::winapi::virtual_query(pages.data());
    if (query.is_err()) return lak::err_t{query.unwrap_err()};
    const auto &info = query.unwrap();
    if (info.State != MEM_COMMIT && info.State != MEM_RESERVE)
      return lak::ok_t{false};
    if (info.RegionSize >= pages.size()) return lak::ok_t{true};
    pages =
      lak::span<void>(lak::span<uint8_t>(pages).subspan(info.RegionSize));
  }
}
#endif