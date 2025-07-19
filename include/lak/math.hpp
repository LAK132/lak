#ifndef LAK_MATH_HPP
#define LAK_MATH_HPP

#include "lak/stdint.hpp"

#include <cmath>
#include <type_traits>

namespace lak
{
	// slack(13, 5) = 2
	template<typename INTEGER>
	constexpr INTEGER slack(INTEGER a, INTEGER b)
	{
		return (b - (a % b)) % b;
	}

	// fslack(11.0, 5.0) = 4.0
	// fslack(14.0, 5.0) = 1.0
	// fslack(-11.0, 5.0) = 1.0
	// fslack(-14.0, 5.0) = 4.0
	template<typename FLOAT>
	FLOAT fslack(FLOAT a, FLOAT b)
	{
		return std::fmod(b - std::fmod(a, b), b);
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

	// fit a to the range [0, b) by adding or subtracting multiples of b
	template<typename INTEGER>
	constexpr INTEGER pmod(INTEGER a, INTEGER b)
	{
		if constexpr (std::is_unsigned_v<INTEGER>)
			return a % b;
		else
			return a > 0U ? a % b : b - ((-a) % b);
	}

	// fit a to the range [0.0, b) by adding or subtracting multiples of b
	template<typename FLOAT>
	constexpr FLOAT fpmod(FLOAT a, FLOAT b)
	{
		return a > 0U ? std::fmod(a, b) : b - std::fmod(-a, b);
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

	// flicks (10^13 W/(sr m^2)/2)
	inline double blackbody_radiance(double wavelength, double temperature)
	{
		constexpr double c     = 2.998E8;
		constexpr double h     = 6.626E-34;
		constexpr double k     = 1.381E-23;
		constexpr double _2hc2 = 2.0 * h * c * c;
		constexpr double _hck  = (h * c) / k;

		const double w5 = std::pow(wavelength, 5.0);
		const double wT = wavelength * temperature;

		return (_2hc2 / w5) / std::expm1(_hck / wT) * 1E-13;
	}

	inline double blackbody_peak_wavelength(double temperature)
	{
		constexpr double b = 2.897771955E-3;
		return b / temperature;
	}

	inline double blackbody_peak_radiance(double temperature)
	{
		return lak::blackbody_radiance(lak::blackbody_peak_wavelength(temperature),
		                               temperature);
	}
}

#endif
