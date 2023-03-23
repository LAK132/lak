#include "lak/alloc.hpp"

#include "lak/compiler.hpp"
#include "lak/math.hpp"
#include "lak/memmanip.hpp"
#include "lak/utility.hpp"

#include <cstddef>
#include <cstdlib>

/* --- local alloc --- */

lak::alloc::result<lak::span<byte_t>> lak::local_alloc(size_t, size_t)
{
	ASSERT_UNREACHABLE();
}

void lak::local_free(lak::span<byte_t>) { ASSERT_UNREACHABLE(); }

/* --- global alloc --- */

lak::alloc::result<lak::span<byte_t>> lak::global_alloc(size_t size,
                                                        size_t align)
{
	if (align < alignof(std::max_align_t)) align = alignof(std::max_align_t);
	void *ptr =
#ifdef LAK_COMPILER_MSVC
	  _aligned_malloc(size, align);
#else
	  std::aligned_alloc(align, size + lak::slack<size_t>(size, align));
#endif
	if (!ptr) return lak::err_t<lak::bad_alloc>{};
	return lak::ok_t{lak::span<byte_t>(static_cast<byte_t *>(ptr), size)};
}

void lak::global_free(lak::span<byte_t> data)
{
#ifdef LAK_COMPILER_MSVC
	_aligned_free(data.data());
#else
	std::free(data.data());
#endif
}
