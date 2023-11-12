#ifndef LAK_MATH_HPP
#define LAK_MATH_HPP

#include "lak/stdint.hpp"

namespace lak
{
	template<typename INTEGER>
	constexpr INTEGER slack(INTEGER a, INTEGER b)
	{
		return (b - (a % b)) % b;
	}

	template<typename INTEGER>
	constexpr INTEGER ceil_div(INTEGER a, INTEGER b)
	{
		return (a / b) + ((a % b) != INTEGER(0) ? INTEGER(1) : INTEGER(0));
	}

	// round a up to a multiple of b
	template<typename INTEGER>
	constexpr INTEGER to_multiple(INTEGER a, INTEGER b)
	{
		return a + lak::slack<INTEGER>(a, b);
	}

	bool close_to(double a, double b, uint32_t epsilon_count = 1);

	bool close_to(float a, float b, uint32_t epsilon_count = 1);

	template<typename INTEGER>
	constexpr INTEGER integer_range_reverse(INTEGER t,
	                                        INTEGER end,
	                                        INTEGER begin = INTEGER(0))
	{
		return (end - (t + INTEGER(1))) + begin;
	}

	static_assert(lak::integer_range_reverse(0, 2) == 1);
	static_assert(lak::integer_range_reverse(1, 2) == 0);
}

#endif
