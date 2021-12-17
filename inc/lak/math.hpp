#ifndef LAK_MATH_HPP
#define LAK_MATH_HPP

#include "lak/stdint.hpp"

namespace lak
{
	template<typename INTEGER>
	INTEGER inv_mod(INTEGER a, INTEGER b)
	{
		return (b - (a % b)) % b;
	}

	bool close_to(double a, double b, uint32_t epsilon_count = 1);

	bool close_to(float a, float b, uint32_t epsilon_count = 1);
}

#endif
