#ifndef LAK_WIDE_MATH_HPP
#define LAK_WIDE_MATH_HPP

#include "lak/stdint.hpp"

namespace lak
{
	struct uint128_t
	{
		uint64_t high;
		uint64_t low;
	};

	inline lak::uint128_t add_u128(uint64_t A, uint64_t B);

	inline lak::uint128_t mul_u128(uint64_t A, uint64_t B);

	struct uintmax2_t
	{
		uintmax_t high;
		uintmax_t low;
	};

	inline lak::uintmax2_t add_uintmax2(uintmax_t A,
	                                    uintmax_t B,
	                                    uintmax_t C = 0U);

	inline lak::uintmax2_t add_uintmax2(lak::uintmax2_t A,
	                                    uintmax_t B,
	                                    uintmax_t C = 0U);

	inline lak::uintmax2_t sub_uintmax2(uintmax_t A, uintmax_t B);

	// A - (B + (~C + 1))
	inline lak::uintmax2_t sub_uintmax2(uintmax_t A, uintmax_t B, uintmax_t C);

	inline lak::uintmax2_t mul_uintmax2(uintmax_t A, uintmax_t B);
}

#include "lak/wide_math.inl"

#endif
