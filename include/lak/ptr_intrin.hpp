#ifndef LAK_PTR_INTRIN_HPP
#define LAK_PTR_INTRIN_HPP

#include "lak/compare.hpp"
#include "lak/compiler.hpp"
#include "lak/memmanip.hpp"

#include <functional>

#if defined(LAK_ARCH_X86_64) || defined(LAK_ARCH_ARM64)
using __lakc_uptrdiff = unsigned long long;
#	define LAKC_UPTRDIFF_MAX 0xFFFF'FFFF'FFFF'FFFFU
#elif defined(LAK_ARCH_X86) || defined(LAK_ARCH_ARM)
using __lakc_uptrdiff = uint32_t;
#	define LAKC_UPTRDIFF_MAX 0xFFFF'FFFFU
#else
#	error Arch not supported
#endif

#if defined(LAK_ARCH_X86_COMPAT)
#	if defined(LAK_COMPILER_MSVC)
#		include <intrin.h>
#	else
#		include <immintrin.h>
#	endif
#endif

struct __lakc_ptr_diff_result
{
	__lakc_uptrdiff diff;
	uint8_t overflow;
};

/* --- a - b --- */

force_inline __lakc_ptr_diff_result __lakc_ptr_diff(const void *a,
                                                    const void *b)
{
	__lakc_ptr_diff_result result;
#if defined(LAK_ARCH_X86_64)
	result.overflow = _subborrow_u64(0U,
	                                 reinterpret_cast<uint64_t>(a),
	                                 reinterpret_cast<uint64_t>(b),
	                                 &result.diff);
#elif defined(LAK_ARCH_X86)
	result.overflow = _subborrow_u32(0U,
	                                 reinterpret_cast<uint32_t>(a),
	                                 reinterpret_cast<uint32_t>(b),
	                                 &result.diff);
#else
	const auto a2{lak::bit_cast<__lakc_uptrdiff>(a)};
	const auto b2{lak::bit_cast<__lakc_uptrdiff>(b)};
	result.diff     = __lakc_uptrdiff(a2 - b2);
	result.overflow = a2 < b2 ? 1U : 0U;
#endif
	return result;
}

/* --- a < b --- */

force_inline bool __lakc_ptr_lt(const void *a, const void *b)
{
	return __lakc_ptr_diff(a, b).overflow > 0U;
}

/* --- a <= b --- */

force_inline bool __lakc_ptr_le(const void *a, const void *b)
{
	__lakc_ptr_diff_result result = __lakc_ptr_diff(a, b);
	return (result.overflow > 0U) |
	       ((result.overflow == 0U) & (result.diff == 0U));
}

/* --- a > b --- */

force_inline bool __lakc_ptr_gt(const void *a, const void *b)
{
	__lakc_ptr_diff_result result = __lakc_ptr_diff(a, b);
	return (result.overflow == 0U) & (result.diff > 0U);
}

/* --- a >= b --- */

force_inline bool __lakc_ptr_ge(const void *a, const void *b)
{
	__lakc_ptr_diff_result result = __lakc_ptr_diff(a, b);
	return (result.overflow == 0U);
}

/* --- a == b --- */

force_inline bool __lakc_ptr_eq(const void *a, const void *b)
{
	__lakc_ptr_diff_result result = __lakc_ptr_diff(a, b);
	return (result.overflow == 0U) & (result.diff == 0U);
}

/* --- a != b --- */

force_inline bool __lakc_ptr_neq(const void *a, const void *b)
{
	__lakc_ptr_diff_result result = __lakc_ptr_diff(a, b);
	return (result.overflow > 0U) | (result.diff > 0U);
}

/* --- begin <= ptr < begin+size --- */

force_inline bool __lakc_ptr_in_range(const void *ptr,
                                      const void *begin,
                                      __lakc_uptrdiff size)
{
	return __lakc_ptr_diff(ptr, begin).diff < size;
}

namespace lak
{
	template<typename T>
	constexpr force_inline bool ptr_in_range(const T *ptr,
	                                         const T *begin,
	                                         size_t size)
	{
		if (std::is_constant_evaluated())
			return !std::less<void>{}(ptr, begin) &&
			       std::less<void>{}(ptr, begin + size);
		else
			return __lakc_ptr_in_range(ptr, begin, size * sizeof(T));
	}

	template<typename T>
	constexpr force_inline bool ptr_in_range(const T *ptr,
	                                         const T *begin,
	                                         const T *end)
	{
		if (std::is_constant_evaluated())
			return !std::less<void>{}(ptr, begin) && std::less<void>{}(ptr, end);
		else
			return __lakc_ptr_in_range(ptr, begin, (end - begin) * sizeof(T));
	}

	template<typename T>
	constexpr force_inline lak::strong_ordering ptr_compare(const T *a,
	                                                        const T *b)
	{
		if (std::is_constant_evaluated())
		{
			return std::less<void>{}(a, b)
			         ? lak::strong_ordering::less
			         : (std::greater<void>{}(a, b) ? lak::strong_ordering::greater
			                                       : lak::strong_ordering::equal);
		}
		else
		{
			__lakc_ptr_diff_result result{__lakc_ptr_diff(a, b)};
			return result.overflow > 0U
			         ? lak::strong_ordering::less
			         : (result.diff > 0U ? lak::strong_ordering::greater
			                             : lak::strong_ordering::equal);
		}
	}
}

#endif
