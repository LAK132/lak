#include "lak/test.hpp"

#include "lak/math.hpp"

#define ASSERT_CLOSE(X, Y, ...)                                               \
	do                                                                          \
	{                                                                           \
		const auto &UNIQUIFY(x) = (X);                                            \
		const auto &UNIQUIFY(y) = (Y);                                            \
		if (!lak::close_to(UNIQUIFY(x), UNIQUIFY(y) __VA_OPT__(, ) __VA_ARGS__))  \
		  [[unlikely]]                                                            \
		{                                                                         \
			FATAL("Assertion '" DEBUG_STRINGIFY_EXPR(                               \
			        lak::close_to(X, Y)) "' failed: '",                             \
			      UNIQUIFY(x),                                                      \
			      "' != '",                                                         \
			      UNIQUIFY(y),                                                      \
			      "'");                                                             \
		}                                                                         \
	} while (false)

BEGIN_TEST(slack)
{
	ASSERT_EQUAL(lak::slack(10U, 5U), 0U);
	ASSERT_EQUAL(lak::slack(11U, 5U), 4U);
	ASSERT_EQUAL(lak::slack(12U, 5U), 3U);
	ASSERT_EQUAL(lak::slack(13U, 5U), 2U);
	ASSERT_EQUAL(lak::slack(14U, 5U), 1U);
	ASSERT_EQUAL(lak::slack(15U, 5U), 0U);
	return 0;
}
END_TEST()

BEGIN_TEST(fslack)
{
	ASSERT_CLOSE(lak::fslack(10.0, 5.0), 0.0);
	ASSERT_CLOSE(lak::fslack(11.0, 5.0), 4.0);
	ASSERT_CLOSE(lak::fslack(12.0, 5.0), 3.0);
	ASSERT_CLOSE(lak::fslack(13.0, 5.0), 2.0);
	ASSERT_CLOSE(lak::fslack(14.0, 5.0), 1.0);
	ASSERT_CLOSE(lak::fslack(15.0, 5.0), 0.0);
	return 0;
}
END_TEST()

BEGIN_TEST(ceil_div)
{
	ASSERT_EQUAL(lak::ceil_div(0U, 2U), 0U);
	ASSERT_EQUAL(lak::ceil_div(1U, 2U), 1U);
	ASSERT_EQUAL(lak::ceil_div(2U, 2U), 1U);
	ASSERT_EQUAL(lak::ceil_div(3U, 2U), 2U);
	ASSERT_EQUAL(lak::ceil_div(4U, 2U), 2U);
	return 0;
}
END_TEST()

BEGIN_TEST(clamp)
{
	ASSERT_EQUAL(lak::clamp(-1, 0, 1), 0);
	ASSERT_EQUAL(lak::clamp(0, 0, 1), 0);
	ASSERT_EQUAL(lak::clamp(1, 0, 1), 1);
	ASSERT_EQUAL(lak::clamp(2, 0, 1), 1);
	return 0;
}
END_TEST()

BEGIN_TEST(frac_to_int)
{
	ASSERT_EQUAL(lak::frac_to_int<uint32_t>(0.0), uint32_t(0));
	ASSERT_EQUAL(lak::frac_to_int<uint32_t>(1.0), uint32_t(UINT32_MAX));

	ASSERT_EQUAL(lak::frac_to_int<int32_t>(-1.0), int32_t(INT32_MIN));
	ASSERT_EQUAL(lak::frac_to_int<int32_t>(0.0), int32_t(0));
	ASSERT_EQUAL(lak::frac_to_int<int32_t>(1.0), int32_t(INT32_MAX));
	return 0;
}
END_TEST()

BEGIN_TEST(int_to_frac)
{
	ASSERT(lak::close_to(lak::int_to_frac<float>(uint8_t(0U)), 0.f));
	ASSERT(lak::close_to(lak::int_to_frac<float>(uint8_t(255U)), 1.f));

	ASSERT(lak::close_to(lak::int_to_frac<float>(int32_t(INT32_MIN)), -1.f));
	ASSERT(lak::close_to(lak::int_to_frac<float>(int32_t(0)), 0.f));
	ASSERT(lak::close_to(lak::int_to_frac<float>(int32_t(INT32_MAX)), 1.f));

	return 0;
}
END_TEST()

BEGIN_TEST(to_multiple)
{
	ASSERT_EQUAL(lak::to_multiple(1U, 10U), 10U);
	ASSERT_EQUAL(lak::to_multiple(5U, 10U), 10U);
	ASSERT_EQUAL(lak::to_multiple(10U, 10U), 10U);
	ASSERT_EQUAL(lak::to_multiple(11U, 10U), 20U);
	ASSERT_EQUAL(lak::to_multiple(15U, 10U), 20U);
	ASSERT_EQUAL(lak::to_multiple(20U, 10U), 20U);
	return 0;
}
END_TEST()

BEGIN_TEST(pmod)
{
	ASSERT_EQUAL(lak::pmod(-11, 10), 9);
	ASSERT_EQUAL(lak::pmod(-10, 10), 0);
	ASSERT_EQUAL(lak::pmod(-9, 10), 1);
	ASSERT_EQUAL(lak::pmod(-5, 10), 5);
	ASSERT_EQUAL(lak::pmod(-1, 10), 9);
	ASSERT_EQUAL(lak::pmod(0, 10), 0);
	ASSERT_EQUAL(lak::pmod(1, 10), 1);
	ASSERT_EQUAL(lak::pmod(5, 10), 5);
	ASSERT_EQUAL(lak::pmod(10, 10), 0);
	ASSERT_EQUAL(lak::pmod(11, 10), 1);
	ASSERT_EQUAL(lak::pmod(15, 10), 5);
	return 0;
}
END_TEST()

BEGIN_TEST(fpmod)
{
	ASSERT_CLOSE(lak::fpmod(-11.0, 10.0), 9.0);
	ASSERT_CLOSE(lak::fpmod(-10.0, 10.0), 0.0);
	ASSERT_CLOSE(lak::fpmod(-9.0, 10.0), 1.0);
	ASSERT_CLOSE(lak::fpmod(-5.0, 10.0), 5.0);
	ASSERT_CLOSE(lak::fpmod(-1.0, 10.0), 9.0);
	ASSERT_CLOSE(lak::fpmod(0.0, 10.0), 0.0);
	ASSERT_CLOSE(lak::fpmod(1.0, 10.0), 1.0);
	ASSERT_CLOSE(lak::fpmod(5.0, 10.0), 5.0);
	ASSERT_CLOSE(lak::fpmod(10.0, 10.0), 0.0);
	ASSERT_CLOSE(lak::fpmod(11.0, 10.0), 1.0);
	ASSERT_CLOSE(lak::fpmod(15.0, 10.0), 5.0);

	ASSERT_CLOSE(lak::fpmod(-11.0, -1.0, 10.0), 0.0);
	ASSERT_CLOSE(lak::fpmod(-10.0, -1.0, 10.0), 1.0);
	ASSERT_CLOSE(lak::fpmod(-9.0, -1.0, 10.0), 2.0);
	ASSERT_CLOSE(lak::fpmod(-5.0, -1.0, 10.0), 6.0);
	ASSERT_CLOSE(lak::fpmod(-2.0, -1.0, 10.0), 9.0);
	ASSERT_CLOSE(lak::fpmod(0.0, 1.0, 10.0), 9.0);
	ASSERT_CLOSE(lak::fpmod(1.0, 1.0, 10.0), 1.0);
	ASSERT_CLOSE(lak::fpmod(5.0, 1.0, 10.0), 5.0);
	ASSERT_CLOSE(lak::fpmod(10.0, 1.0, 10.0), 1.0);
	ASSERT_CLOSE(lak::fpmod(11.0, 2.0, 10.0), 3.0);
	ASSERT_CLOSE(lak::fpmod(15.0, 3.0, 10.0), 8.0);
	return 0;
}
END_TEST()

// BEGIN_TEST(sigmoid)
// {
// 	return 0;
// }
// END_TEST()

BEGIN_TEST(close_to)
{
	ASSERT(lak::close_to(1.0, 1.0));
	ASSERT(!lak::close_to(2.0, 1.0));
	ASSERT(!lak::close_to(1.0, 1.1));
	return 0;
}
END_TEST()

// BEGIN_TEST(integer_range_reverse)
// {
// 	return 0;
// }
// END_TEST()

// BEGIN_TEST(blackbody)
// {
// 	{
// 		// lak::blackbody_radiance
// 	}
// 	{
// 		// lak::earth_sun_blackbody_irradiance
// 	}
// 	{
// 		// lak::blackbody_peak_wavelength
// 	}
// 	{
// 		// lak::blackbody_peak_radiance
// 	}
// 	return 0;
// }
// END_TEST()
