#ifndef LAK_MATH_HPP
#define LAK_MATH_HPP

#include "lak/stdint.hpp"

#include <algorithm>
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

	template<typename NUMBER>
	constexpr NUMBER clamp(NUMBER x, NUMBER min, NUMBER max)
	{
		return std::min<NUMBER>(std::max<NUMBER>(x, min), max);
	}

	// FLOAT [0.0, 1.0] -> unsigned INTEGER [0, MAX]
	// FLOAT [-1.0, 1.0] -> signed INTEGER [MIN, MAX]
	template<typename INTEGER, typename FLOAT>
	constexpr INTEGER frac_to_int(FLOAT f)
	{
		if constexpr (std::is_unsigned_v<INTEGER>)
			return static_cast<INTEGER>(lak::clamp<long long>(
			  std::llround(lak::clamp<FLOAT>(f, FLOAT(0), FLOAT(1)) *
			               FLOAT(std::numeric_limits<INTEGER>::max())),
			  std::numeric_limits<INTEGER>::lowest(),
			  std::numeric_limits<INTEGER>::max()));
		else if (f >= FLOAT(0))
			return static_cast<INTEGER>(lak::clamp<long long>(
			  std::llround(lak::clamp<FLOAT>(f, FLOAT(0), FLOAT(1)) *
			               FLOAT(std::numeric_limits<INTEGER>::max())),
			  INTEGER(0),
			  std::numeric_limits<INTEGER>::max()));
		else
			return static_cast<INTEGER>(lak::clamp<long long>(
			  std::llround(lak::clamp<FLOAT>(-f, FLOAT(0), FLOAT(1)) *
			               FLOAT(std::numeric_limits<INTEGER>::lowest())),
			  std::numeric_limits<INTEGER>::lowest(),
			  INTEGER(0)));
	}

	// unsigned INTEGER [0, MAX] -> FLOAT [0.0, 1.0]
	// signed INTEGER [MIN, MAX] -> FLOAT [-1.0, 1.0]
	template<typename FLOAT, typename INTEGER>
	constexpr FLOAT int_to_frac(INTEGER i)
	{
		if constexpr (std::is_unsigned_v<INTEGER>)
			return lak::clamp(FLOAT(i) / FLOAT(std::numeric_limits<INTEGER>::max()),
			                  FLOAT(0),
			                  FLOAT(1));
		else if (i >= INTEGER(0))
			return lak::clamp(FLOAT(i) / FLOAT(std::numeric_limits<INTEGER>::max()),
			                  FLOAT(0),
			                  FLOAT(1));
		else
			return lak::clamp(FLOAT(i) /
			                    -FLOAT(std::numeric_limits<INTEGER>::lowest()),
			                  FLOAT(-1),
			                  FLOAT(0));
	}

	// unsigned INTEGER [0, max] -> FLOAT [0.0, 1.0]
	// signed INTEGER [-max, max] -> FLOAT [-1.0, 1.0]
	template<typename FLOAT, typename INTEGER>
	constexpr FLOAT int_to_frac(INTEGER i, INTEGER i_max)
	{
		if constexpr (std::is_unsigned_v<INTEGER>)
			return lak::clamp(FLOAT(i) / FLOAT(i_max), FLOAT(0), FLOAT(1));
		else if (i >= INTEGER(0))
			return lak::clamp(FLOAT(i) / FLOAT(i_max), FLOAT(0), FLOAT(1));
		else
			return lak::clamp(FLOAT(i) / -FLOAT(i_max), FLOAT(-1), FLOAT(0));
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
			return a >= INTEGER(0) ? (a % b) : lak::slack<INTEGER>(-a, b);
	}

	// fit value to the range [0.0, range_max) by adding or subtracting multiples
	// of range_max
	template<typename FLOAT>
	constexpr FLOAT fpmod(FLOAT value, FLOAT range_max)
	{
		return value >= FLOAT(0) ? std::fmod(value, range_max)
		                         : lak::fslack<FLOAT>(-value, range_max);
	}

	// fit value to the range [range_min, range_max) by adding or subtracting
	// multiples of range_max-range_min
	template<typename FLOAT>
	constexpr FLOAT fpmod(FLOAT value, FLOAT range_min, FLOAT range_max)
	{
		return lak::fpmod<FLOAT>(value - range_min, range_max - range_min) +
		       range_min;
	}

	// https://dinodini.wordpress.com/2010/04/05/normalized-tunable-sigmoid-functions/
	template<typename FLOAT>
	constexpr FLOAT _half_sigmoid(FLOAT k, FLOAT t)
	{
		return (k * t) / (FLOAT(1) + k - t);
	}

	template<typename FLOAT>
	constexpr FLOAT _sigmoid(FLOAT k, FLOAT t)
	{
		return std::copysign(
		  lak::_half_sigmoid(k, std::min<FLOAT>(std::fabs(t), FLOAT(1))), t);
	}

	template<typename FLOAT>
	constexpr auto sigmoid_generator(FLOAT k)
	{
		k = k < FLOAT(0) ? std::min<FLOAT>(k, -FLOAT(1.0001))
		                 : std::max<FLOAT>(k, FLOAT(0.0001));
		return [k](FLOAT t) -> FLOAT { return lak::_sigmoid(k, t); };
	}

	template<typename FLOAT>
	constexpr FLOAT sigmoid(FLOAT k, FLOAT t)
	{
		return lak::sigmoid_generator(k)(t);
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

	// wavelength = nanometers
	// temperature = kelvins
	// result = W/sr/m^2/nm = deciflicks
	inline double blackbody_radiance(double wavelength, double temperature)
	{
		constexpr double c     = 2.998E8;
		constexpr double h     = 6.626E-34;
		constexpr double k     = 1.381E-23;
		constexpr double _2hc2 = 2.0 * h * c * c;
		constexpr double _hck  = (h * c) / k;

		wavelength *= 1E-9;

		const double w5 = std::pow(wavelength, 5.0);
		const double wT = wavelength * temperature;

		return ((_2hc2 / w5) / std::expm1(_hck / wT)) * 1E-9;
	}

	constexpr double suns_steradian = 6E-5;

	// earth irradiance (W/m^2/nm) = blackbody (W/sr/m^2/nm) * sun's steradian
	// wavelength = nanometers
	// temperature = kelvins
	// result = W/m^2/nm
	inline double earth_sun_blackbody_irradiance(double wavelength,
	                                             double temperature)
	{
		return lak::blackbody_radiance(wavelength, temperature) * suns_steradian;
	}

	// temperature = kelvins
	// result = nanometers
	inline double blackbody_peak_wavelength(double temperature)
	{
		constexpr double b = 2.897771955E-3;
		return (b / temperature) * 1E9;
	}

	// temperature = kelvins
	// result = W/sr/m^2/nm = deciflicks
	inline double blackbody_peak_radiance(double temperature)
	{
		return lak::blackbody_radiance(lak::blackbody_peak_wavelength(temperature),
		                               temperature);
	}
}

#endif
