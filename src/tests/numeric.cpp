#include "lak/test.hpp"

#include "lak/numeric.hpp"

BEGIN_TEST(string_to_number)
{
	{
		auto dub = lak::dec_string_to_double(u8"-1", u8"0", {}).UNWRAP();
		ASSERT_EQUAL(dub, -1.0);
	}

	return 0;
}
END_TEST()

BEGIN_TEST(mul_uintmax2)
{
	DEBUG_EXPR(lak::mul_uintmax2(uintmax_t(2), UINTMAX_MAX).low);
	DEBUG_EXPR(lak::mul_uintmax2(uintmax_t(2), UINTMAX_MAX).high);
	DEBUG_EXPR(lak::mul_uintmax2(uintmax_t(-2), UINTMAX_MAX).low);
	DEBUG_EXPR(lak::mul_uintmax2(uintmax_t(-2), UINTMAX_MAX).high);

	auto v = lak::mul_uintmax2(uintmax_t(-2), UINTMAX_MAX);
	v.low  = ~v.low;
	v.high = ~v.high;
	v      = lak::add_uintmax2(v, 1U);
	DEBUG_EXPR(v.low);
	DEBUG_EXPR(v.high);

	return 0;
}
END_TEST()
