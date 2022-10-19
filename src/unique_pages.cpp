#include "lak/unique_pages.hpp"

#include "lak/result.hpp"

#include "lak/memmanip.hpp"

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
