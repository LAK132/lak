#include "lak/system/page_memory.hpp"
#include "lak/math.hpp"
#include "lak/result.hpp"
#include "lak/span.hpp"

// needed here to fix missing symbol issues
#include "lak/streamify.hpp"

size_t lak::round_to_page_multiple(size_t size, size_t *page_size_out)
{
	const size_t page_size = lak::page_size();
	if (page_size_out) *page_size_out = page_size;
	return size + lak::slack<size_t>(size, page_size);
}

void lak::unique_pages::clear()
{
	if (!empty()) lak::page_free(*this).expect("free failed");
	static_cast<lak::span<void> &>(*this) = {};
}

lak::unique_pages lak::unique_pages::make(size_t min_size, size_t *actual_size)
{
	return lak::unique_pages(
	  lak::page_reserve(min_size, actual_size).expect("reserve failed"));
}
