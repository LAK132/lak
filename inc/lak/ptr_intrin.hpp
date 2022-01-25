#ifndef LAK_PTR_INTRIN_HPP
#define LAK_PTR_INTRIN_HPP

#include "lak/compare.hpp"
#include "lak/compiler.hpp"
#include "lak/intrin.hpp"

#include <functional>

#if defined(LAK_ARCH_X86_64)
using __lakc_uptrdiff = unsigned long long;
#	define LAKC_UPTRDIFF_MAX 0xFFFF'FFFF'FFFF'FFFFU
#elif defined(LAK_ARCH_X86)
using __lakc_uptrdiff = uint32_t;
#	define LAKC_UPTRDIFF_MAX 0xFFFF'FFFFU
#else
#	error Arch not supported
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
#	error Compiler/OS/Arch not supported
#endif
	return result;
}

/* --- a < b --- */

constexpr force_inline bool __lakc_ptr_lt(const void *a, const void *b)
{
	if (std::is_constant_evaluated())
		return std::less<const void *>{}(a, b);
	else
		return __lakc_ptr_diff(a, b).overflow > 0U;
}

/* --- a <= b --- */

constexpr force_inline bool __lakc_ptr_le(const void *a, const void *b)
{
	if (std::is_constant_evaluated())
		return !std::less<const void *>{}(b, a);
	else
	{
		__lakc_ptr_diff_result result = __lakc_ptr_diff(a, b);
		return (result.overflow > 0U) |
		       ((result.overflow == 0U) & (result.diff == 0U));
	}
}

/* --- a > b --- */

constexpr force_inline bool __lakc_ptr_gt(const void *a, const void *b)
{
	if (std::is_constant_evaluated())
		return std::less<const void *>{}(b, a);
	else
	{
		__lakc_ptr_diff_result result = __lakc_ptr_diff(a, b);
		return (result.overflow == 0U) & (result.diff > 0U);
	}
}

/* --- a >= b --- */

constexpr force_inline bool __lakc_ptr_ge(const void *a, const void *b)
{
	if (std::is_constant_evaluated())
		return !std::less<const void *>{}(a, b);
	else
	{
		__lakc_ptr_diff_result result = __lakc_ptr_diff(a, b);
		return (result.overflow == 0U) & (result.diff >= 0U);
	}
}

/* --- a == b --- */

constexpr force_inline bool __lakc_ptr_eq(const void *a, const void *b)
{
	if (std::is_constant_evaluated())
		return !std::less<const void *>{}(a, b) &&
		       !std::less<const void *>{}(b, a);
	else
	{
		__lakc_ptr_diff_result result = __lakc_ptr_diff(a, b);
		return (result.overflow == 0U) & (result.diff == 0U);
	}
}

/* --- a != b --- */

constexpr force_inline bool __lakc_ptr_neq(const void *a, const void *b)
{
	if (std::is_constant_evaluated())
		return std::less<const void *>{}(a, b) || std::less<const void *>{}(b, a);
	else
	{
		__lakc_ptr_diff_result result = __lakc_ptr_diff(a, b);
		return (result.overflow > 0U) | (result.diff > 0U);
	}
}

/* --- begin <= ptr < begin+size --- */

constexpr force_inline bool __lakc_ptr_in_range(const void *ptr,
                                                const void *begin,
                                                __lakc_uptrdiff size)
{
	if (std::is_constant_evaluated())
	{
		if (size == 0) return false;

		if (const void *end{reinterpret_cast<const char *>(begin) + size};
		    __lakc_ptr_lt(begin, end))
			return __lakc_ptr_ge(ptr, begin) && __lakc_ptr_lt(ptr, end);
		else
			return __lakc_ptr_ge(ptr, begin) || __lakc_ptr_lt(ptr, end);
	}
	else
	{
		return __lakc_ptr_diff(ptr, begin).diff < size;
	}
}

namespace lak
{
	template<typename T>
	constexpr force_inline bool ptr_in_range(const T *ptr,
	                                         const T *begin,
	                                         size_t size)
	{
		return __lakc_ptr_in_range(static_cast<const void *>(ptr),
		                           static_cast<const void *>(begin),
		                           size * sizeof(T));
	}

	template<typename T>
	constexpr force_inline bool ptr_in_range(const T *ptr,
	                                         const T *begin,
	                                         const T *end)
	{
		return __lakc_ptr_in_range(static_cast<const void *>(ptr),
		                           static_cast<const void *>(begin),
		                           (end - begin) * sizeof(T));
	}
}

#endif
