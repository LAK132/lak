#ifndef LAK_PTR_INTRIN_HPP
#define LAK_PTR_INTRIN_HPP

#include "lak/compiler.hpp"
#include "lak/intrin.hpp"

#if defined(LAK_ARCH_X86_64)
using __lakc_uptrdiff = unsigned long long;
#elif defined(LAK_ARCH_X86)
using __lakc_uptrdiff = uint32_t;
#else
#	error Arch not supported
#endif

struct __lakc_ptr_diff_result
{
	__lakc_uptrdiff diff;
	uint8_t overflow;
};

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
#	error Compiler/OS/Arch not supported
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
	return (result.overflow == 0U) & (result.diff >= 0U);
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

/* --- a <= p < a+s --- */

force_inline bool __lakc_ptr_in_range(const void *p,
                                      const void *a,
                                      __lakc_uptrdiff s)
{
	__lakc_ptr_diff_result result = __lakc_ptr_diff(p, a);
	return (result.overflow == 0U) & (result.diff < s);
}

#endif
