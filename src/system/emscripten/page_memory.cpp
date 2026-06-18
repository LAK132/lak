#include "lak/system/page_memory.hpp"
#include "lak/math.hpp"
#include "lak/result.hpp"
#include "lak/span.hpp"
#include "lak/system/compiler.hpp"
#include "lak/system/os.hpp"

#include "assert.h"

#include "emscripten/emmalloc.h"

size_t lak::page_size() { return 4096U; }

lak::page_result_t<lak::span<void>> lak::page_reserve(size_t size,
                                                      size_t *page_size_out)
{
	const size_t ps = lak::page_size();
	size            = lak::to_multiple(size, ps);
	void *p         = emmalloc_memalign(ps, size);
	if (page_size_out) *page_size_out = ps;
	return lak::ok_t{lak::span<void>(p, size)};
}

lak::page_result_t<> lak::page_commit(lak::span<void>) { return lak::ok_t{}; }

lak::page_result_t<> lak::page_decommit(lak::span<void>)
{
	return lak::ok_t{};
}

lak::page_result_t<> lak::page_free(lak::span<void> pages)
{
	assert(emmalloc_usable_size(pages.data()) >= pages.size());
	emmalloc_free(pages.data());
	return lak::ok_t{};
}
