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

	bool close_to(double a, double b, uint32_t epsilon_count = 1);

	bool close_to(float a, float b, uint32_t epsilon_count = 1);
}

#endif
